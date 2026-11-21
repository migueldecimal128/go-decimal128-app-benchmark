// SPDX-License-Identifier: MIT
//go:build go_builtin_decimal128_xorbid && go1.28

package decimal

// Library identity, recorded with the results: Marcelo Cantos's
// toolchain's decimal128, stored as XOR-BID (see
// lib_go_builtin_decimal128.go).
const (
	Name     = "go-builtin-decimal128-xorbid"
	Module   = "std (GOEXPERIMENT=decimal, XOR-BID)"
	BuildTag = "go_builtin_decimal128_xorbid"
)
