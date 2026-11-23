// SPDX-License-Identifier: MIT

//go:build goexperiment.decimal && go1.28

package microbench

import "testing"

// BenchmarkOps times each operation at both widths on the same values:
// Op/Width/Class, with the classes described in README.md.
func BenchmarkOps(b *testing.B) {
	b.Run("64", benchOps64)
	b.Run("128", benchOps128)
}

// BenchmarkMem streams over slices of n money values (8n or 16n bytes),
// from L1-sized to DRAM-sized.
func BenchmarkMem(b *testing.B) {
	for _, n := range []int{1 << 10, 1 << 14, 1 << 17, 1 << 20, 1 << 24} {
		benchMem64(b, n)
		benchMem128(b, n)
	}
}

// BenchmarkGather sums 1M values read at random indices from a table of n,
// so that the table's size decides which cache level holds it.
func BenchmarkGather(b *testing.B) {
	for _, n := range []int{1 << 12, 1 << 16, 1 << 18, 1 << 20, 1310720, 1572864, 1835008,
		1 << 21, 2621440, 3145728, 3670016, 1 << 22, 1 << 24} {
		r := uint64(88172645463325252)
		idx := make([]int32, 1<<20)
		for i := range idx {
			r ^= r << 13
			r ^= r >> 7
			r ^= r << 17
			idx[i] = int32(r % uint64(n))
		}
		benchGather64(b, n, idx)
		benchGather128(b, n, idx)
	}
}

// BenchmarkSort sorts n values with a comparison function.
func BenchmarkSort(b *testing.B) {
	for _, n := range []int{1 << 10, 1 << 20} {
		benchSort64(b, n)
		benchSort128(b, n)
	}
}

// The pools hold the same values at both widths, so that every row of one
// width has a counterpart doing the same work on the same numbers.
func TestSameValues(t *testing.T) {
	x64, y64 := pool64(1024, 42)
	x128, y128 := pool128(1024, 42)
	for i := range x64 {
		if decimal128(x64[i]) != x128[i] || decimal128(y64[i]) != y128[i] {
			t.Fatalf("d16 pool %d: %v != %v", i, x64[i], x128[i])
		}
	}
	for _, neg := range []int{0, 2, 10} {
		a, b, c := centsPoolNeg64(1024, 43, neg)
		x, y, z := centsPoolNeg128(1024, 43, neg)
		for i := range a {
			if decimal128(a[i]) != x[i] || decimal128(b[i]) != y[i] || decimal128(c[i]) != z[i] {
				t.Fatalf("cents pool (1 in %d negative) %d differs", neg, i)
			}
		}
	}
}
