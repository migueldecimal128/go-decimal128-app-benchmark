// SPDX-License-Identifier: MIT
//go:build go_builtin_decimal128_ubd_level3 && go1.28

package decimal

// Library identity, recorded with the results: the ubd128 toolchain's
// decimal128 compiled with -d=ubdlevel=3, the inline paths derived from
// the BID lowering (see lib_go_builtin_decimal128.go). Until the fork's
// 45ff73c327 this was level -1, recorded as
// go-builtin-decimal128-ubd-level-1.
const (
	Name     = "go-builtin-decimal128-ubd-level3"
	Module   = "std (GOEXPERIMENT=decimal, UBD, -d=ubdlevel=3)"
	BuildTag = "go_builtin_decimal128_ubd_level3"
)
