// SPDX-License-Identifier: GPL-3.0-or-later
// tax_preparation CA harness on the d128 backend (decimal128-go).
package main

import (
	"os"

	"github.com/migueldecimal128/go-decimal128-app-benchmark/apps/tax_preparation/internal/tax"
)

func main() { os.Exit(tax.RunCA()) }
