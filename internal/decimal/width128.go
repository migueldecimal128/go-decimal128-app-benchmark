// SPDX-License-Identifier: MIT
//go:build !go_builtin_decimal64

package decimal

// Width is the library type's width in bits. Every library but
// go-builtin-decimal64 is decimal128, and the apps read their 128-bit inputs.
const Width = 128

// FromBID64 is only for a 64-bit library (Width 64), whose apps read the
// suite's 64-bit inputs; the apps do not call it otherwise.
func FromBID64(b uint64) Decimal128 { panic("decimal: FromBID64 on a 128-bit library") }
