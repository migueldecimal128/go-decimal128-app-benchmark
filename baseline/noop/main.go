// SPDX-License-Identifier: MIT

// Command noop does nothing. The runner times it, launched the same way as the
// benchmark programs, as the startup baseline: process creation plus Go
// runtime start-up and exit.
package main

func main() {}
