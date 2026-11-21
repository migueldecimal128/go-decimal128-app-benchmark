// SPDX-License-Identifier: MIT
//go:build go_builtin_decimal128 && go1.28

package decimal

// Library identity, recorded with the results: the ubd128 toolchain's
// decimal128 (see lib_go_builtin_decimal128.go).
const (
	Name     = "go-builtin-decimal128"
	Module   = "std (GOEXPERIMENT=decimal, UBD)"
	BuildTag = "go_builtin_decimal128"
)
