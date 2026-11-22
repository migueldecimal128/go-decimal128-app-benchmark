// SPDX-License-Identifier: MIT

//go:build ignore

// bid64 writes testGen/bin/Banking.bin.BID64, the 643 commands of
// Banking.bin.BID128 with their two amounts as BID decimal64, for a 64-bit
// library. Run from data/banking:
//
//	go run testGen/bid64.go
//
// A decimal128 command record is 64 bytes: a 16-byte name, the fund and str1
// amounts (16 little-endian BID bytes each) and four little-endian int32s. A
// decimal64 record is 48: the same, with 8-byte amounts. Each amount keeps its
// sign, coefficient and exponent exactly; the program fails if one does not
// fit decimal64 (every amount has at most 9 digits).
//
// The suite's own 64-bit command file, Banking.bin.BID, holds 1238 other
// commands, so it would time different work from the 128-bit row.
package main

import (
	"encoding/binary"
	"fmt"
	"os"
)

func main() {
	in, err := os.ReadFile("testGen/bin/Banking.bin.BID128")
	if err != nil {
		fail(err)
	}
	if len(in)%64 != 0 {
		fail(fmt.Errorf("Banking.bin.BID128: %d bytes, not a whole number of 64-byte records", len(in)))
	}
	out := make([]byte, 0, len(in)/64*48)
	for r := 0; r < len(in); r += 64 {
		rec := in[r : r+64]
		out = append(out, rec[:16]...)
		for _, off := range []int{16, 32} {
			lo := binary.LittleEndian.Uint64(rec[off:])
			hi := binary.LittleEndian.Uint64(rec[off+8:])
			out = binary.LittleEndian.AppendUint64(out, to64(hi, lo, r/64))
		}
		out = append(out, rec[48:64]...)
	}
	if err := os.WriteFile("testGen/bin/Banking.bin.BID64", out, 0o644); err != nil {
		fail(err)
	}
	fmt.Printf("testGen/bin/Banking.bin.BID64: %d commands\n", len(in)/64)
}

// to64 re-encodes the finite BID decimal128 (hi, lo) as BID decimal64.
func to64(hi, lo uint64, rec int) uint64 {
	if hi>>61&3 == 3 {
		fail(fmt.Errorf("record %d: not a finite value with a first-form encoding", rec))
	}
	sign := hi >> 63
	exp := int(hi>>49&0x3fff) - 6176
	if hi&(1<<49-1) != 0 || lo >= 1e16 || exp < -398 || exp > 369 {
		fail(fmt.Errorf("record %d: coefficient or exponent %d outside decimal64", rec, exp))
	}
	if lo < 1<<53 {
		return sign<<63 | uint64(exp+398)<<53 | lo
	}
	return sign<<63 | 3<<61 | uint64(exp+398)<<51 | lo&(1<<51-1)
}

func fail(err error) {
	fmt.Fprintln(os.Stderr, "bid64:", err)
	os.Exit(1)
}
