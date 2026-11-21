// SPDX-License-Identifier: MIT
//go:build go_builtin_decimal128_ubd_level1 && go1.28

package decimal

// Library identity, recorded with the results: the ubd128 toolchain's
// decimal128 compiled with -d=ubdlevel=1, every operation a call into the
// kernel with nothing inline (see lib_go_builtin_decimal128.go).
const (
	Name     = "go-builtin-decimal128-ubd-level1"
	Module   = "std (GOEXPERIMENT=decimal, UBD, -d=ubdlevel=1)"
	BuildTag = "go_builtin_decimal128_ubd_level1"
)
