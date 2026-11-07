// SPDX-License-Identifier: MIT
//go:build !miguel_decimal128_go && !bid754

// Compiled only when no library's build tag is given (each library's tag is
// listed on the line above as !<tag>). There is no default library: every
// build must name one, e.g. go build -tags miguel_decimal128_go ./apps/...
// This file exists to make a build without a tag fail with a clear message.

package dec

var _ = noLibrarySelected_buildWithTags_miguel_decimal128_go_or_bid754
