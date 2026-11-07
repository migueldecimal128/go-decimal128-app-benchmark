// SPDX-License-Identifier: MIT

package main

import (
	"fmt"
	"os"
	"strings"
	"testing"
)

// TestSmoke runs Cowlishaw's ten-call worked example (telco.testb) and checks
// every output line and the published control totals.
func TestSmoke(t *testing.T) {
	// A library that lacks an operation telco needs panics with "does not
	// support"; that is a skip, not a failure.
	defer func() {
		if r := recover(); r != nil {
			if msg := fmt.Sprint(r); strings.Contains(msg, "does not support") {
				t.Skip(msg)
			}
			panic(r)
		}
	}()
	in, err := os.ReadFile("../../../../data/telco/telco.testb")
	if err != nil {
		t.Fatal(err)
	}
	want, err := os.ReadFile("../../../../data/telco/golden/telco.expected")
	if err != nil {
		t.Fatal(err)
	}
	out, tot := bill(in, nil)
	if string(out) != string(want) {
		t.Errorf("output:\n%s\nwant:\n%s", out, want)
	}
	totals, err := os.ReadFile("../../../../data/telco/golden/telco.totals")
	if err != nil {
		t.Fatal(err)
	}
	got := "sumT=" + tot.t.String() + "\nsumB=" + tot.b.String() + "\nsumD=" + tot.d.String() + "\n"
	if got != string(totals) {
		t.Errorf("totals:\n%swant:\n%s", got, strings.TrimSpace(string(totals)))
	}
}
