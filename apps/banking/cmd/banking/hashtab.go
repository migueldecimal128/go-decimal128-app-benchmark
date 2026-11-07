// SPDX-License-Identifier: GPL-3.0-or-later
//
// Faithful port of Bob Jenkins' lookupa.c (== lookup2) + hashtab.c (chained open
// hash), from apps/banking/c/src via the Swift HashTable.swift. Public Domain
// (Jenkins); its presence in the GPL banking arm does not change its origin.
//
// WHY a hand port and not a Go map: the banking transcript prints accounts in
// hash-table ITERATION ORDER (endOfTheDay* sweep hfirst/hnext over each table,
// and the end-of-run drain lists them). That order is a function of the exact
// Jenkins hash value, the table size (2^logsize), bucket = (hval & mask), the
// prepend-on-insert chaining, and the bucket-ascending / within-bucket-head-first
// walk — none of which a map reproduces.
package main

// bankLookup hashes a byte key (lookupa.c). CRITICAL: `ub4` in standard.h is
// `unsigned long int`, which on LP64 (where the golden was generated) is
// 64-bit, so the hash runs in 64-bit wrapping arithmetic and bucket = hval &
// mask sees the full 64-bit value. A 32-bit version gives a different bucket
// layout and therefore a different iteration order.
func bankLookup(k string, length int, level uint64) uint64 {
	a, b, c := uint64(0x9e3779b9), uint64(0x9e3779b9), level
	p, n := 0, length
	mix := func() {
		a -= b
		a -= c
		a ^= c >> 13
		b -= c
		b -= a
		b ^= a << 8
		c -= a
		c -= b
		c ^= b >> 13
		a -= b
		a -= c
		a ^= c >> 12
		b -= c
		b -= a
		b ^= a << 16
		c -= a
		c -= b
		c ^= b >> 5
		a -= b
		a -= c
		a ^= c >> 3
		b -= c
		b -= a
		b ^= a << 10
		c -= a
		c -= b
		c ^= b >> 15
	}
	u := func(i int) uint64 { return uint64(k[i]) }
	for n >= 12 {
		a += u(p) + (u(p+1) << 8) + (u(p+2) << 16) + (u(p+3) << 24)
		b += u(p+4) + (u(p+5) << 8) + (u(p+6) << 16) + (u(p+7) << 24)
		c += u(p+8) + (u(p+9) << 8) + (u(p+10) << 16) + (u(p+11) << 24)
		mix()
		p += 12
		n -= 12
	}
	c += uint64(length)
	// switch(len), all cases fall through (add trailing bytes high-to-low).
	if n >= 11 {
		c += u(p+10) << 24
	}
	if n >= 10 {
		c += u(p+9) << 16
	}
	if n >= 9 {
		c += u(p+8) << 8
	}
	if n >= 8 {
		b += u(p+7) << 24
	}
	if n >= 7 {
		b += u(p+6) << 16
	}
	if n >= 6 {
		b += u(p+5) << 8
	}
	if n >= 5 {
		b += u(p + 4)
	}
	if n >= 4 {
		a += u(p+3) << 24
	}
	if n >= 3 {
		a += u(p+2) << 16
	}
	if n >= 2 {
		a += u(p+1) << 8
	}
	if n >= 1 {
		a += u(p)
	}
	mix()
	return c
}

// --- chained hash table (hashtab.c) -------------------------------------------

type hitem struct {
	key   string
	stuff any
	hval  uint64
	next  *hitem
}

type htab struct {
	table   []*hitem
	logsize uint
	mask    uint64
	count   int
	apos    uint64
	ipos    *hitem
}

// newHtab is hcreate: initial size 2^logsize.
func newHtab(logsize uint) *htab {
	n := 1 << logsize
	return &htab{table: make([]*hitem, n), logsize: logsize, mask: uint64(n - 1)}
}

func (t *htab) hstuff() any { return t.ipos.stuff }

// grow is hgrow: double the table, rehash every node (prepend into the new bucket).
func (t *htab) grow() {
	t.logsize++
	newsize := 1 << t.logsize
	newmask := uint64(newsize - 1)
	newtab := make([]*hitem, newsize)
	for i := (newsize >> 1) - 1; i >= 0; i-- {
		for this := t.table[i]; this != nil; {
			that := this
			this = that.next
			idx := that.hval & newmask
			that.next = newtab[idx]
			newtab[idx] = that
		}
	}
	t.table = newtab
	t.mask = newmask
	t.hfirst()
}

// hfind moves the current position to a key; true if present. The C hashes
// strlen(acct_no) bytes (no NUL), so the key is the account string itself.
func (t *htab) hfind(key string) bool {
	x := bankLookup(key, len(key), 0)
	y := x & t.mask
	for h := t.table[y]; h != nil; h = h.next {
		if (h.hval == x) && (h.key == key) {
			t.apos, t.ipos = y, h
			return true
		}
	}
	return false
}

// hadd inserts; false if the key is already present.
func (t *htab) hadd(key string, stuff any) bool {
	x := bankLookup(key, len(key), 0)
	y := x & t.mask
	for h := t.table[y]; h != nil; h = h.next {
		if (h.hval == x) && (h.key == key) {
			t.apos, t.ipos = y, h
			return false
		}
	}
	node := &hitem{key: key, stuff: stuff, hval: x}
	t.count++
	if t.count > (1 << t.logsize) {
		t.grow()
		y = x & t.mask
	}
	node.next = t.table[y]
	t.table[y] = node
	t.ipos, t.apos = node, y
	return true
}

// hdel deletes the item at the current position and advances to the next.
func (t *htab) hdel() bool {
	h := t.ipos
	if h == nil {
		return false
	}
	if cur := t.table[t.apos]; cur == h {
		t.table[t.apos] = h.next
	} else {
		for (cur != nil) && (cur.next != h) {
			cur = cur.next
		}
		if cur != nil {
			cur.next = h.next
		}
	}
	t.count--
	t.ipos = h.next
	if t.ipos == nil {
		t.hnbucket()
	}
	return true
}

// hfirst positions on the first element in the table.
func (t *htab) hfirst() bool {
	t.apos = t.mask
	t.hnbucket()
	return t.ipos != nil
}

// hnext (macro): follow ->next, else advance to the next nonempty bucket.
func (t *htab) hnext() bool {
	if t.ipos == nil {
		return false
	}
	t.ipos = t.ipos.next
	if t.ipos != nil {
		return true
	}
	return t.hnbucket()
}

// hnbucket moves to the first item in the next nonempty bucket; false when it
// wraps around to the beginning.
func (t *htab) hnbucket() bool {
	oldapos := t.apos
	end := uint64(1) << t.logsize
	for i := oldapos + 1; i < end; i++ {
		if node := t.table[i&t.mask]; node != nil {
			t.apos, t.ipos = i, node
			return true
		}
	}
	for i := uint64(0); i <= oldapos; i++ {
		if node := t.table[i]; node != nil {
			t.apos, t.ipos = i, node
			return false
		}
	}
	return false
}
