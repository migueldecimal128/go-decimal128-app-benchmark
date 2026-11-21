// SPDX-License-Identifier: MIT
//go:build !miguel_decimal128_go && !bid754 && !go_decimal && !go_builtin_decimal128 && !go_builtin_decimal128_xorbid && !go_builtin_decimal128_ubd_level3 && !go_builtin_decimal128_ubd_level1

// Compiled only when no library's build tag is given (each library's tag is
// listed on the line above as !<tag>). There is no default library: every
// build must name one, e.g. go build -tags miguel_decimal128_go ./apps/...
// This file exists to make a build without a tag fail with a clear message.

package decimal

var _ = noLibrarySelected_buildWithTags_miguel_decimal128_go_or_bid754_or_go_decimal_or_go_builtin_decimal128
