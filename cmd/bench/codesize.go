// SPDX-License-Identifier: MIT

package main

import (
	"bufio"
	"bytes"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"strings"
)

// codeBytes is the size of a program's own machine code: the instructions of
// its main package and of this repository's packages (the adapter and the
// harness), where a library's operations are compiled and, for the built-in
// decimal types, inlined. The library's own code, the runtime and the
// standard library are left out: they are shared by every program and do
// not grow with the call sites. It is the sum of the instruction bytes go
// tool objdump lists for those functions, without the padding after a
// function (arm64's undecodable zero words, amd64's INT3 fill), so a
// different function alignment does not change it.
//
// goBin and env are the library's go command (any go command reads any
// program; the library's is the one known to exist). It returns 0 when
// objdump fails, and the report then shows no size.
func codeBytes(goBin string, env []string, root, exe string) int64 {
	module := modulePath(root)
	if module == "" {
		return 0
	}
	cmd := exec.Command(goBin, "tool", "objdump", "-s", `^(main\.|`+regexp.QuoteMeta(module)+`/)`, exe)
	cmd.Env = env
	out, err := cmd.Output()
	if err != nil {
		return 0
	}
	var n int64
	sc := bufio.NewScanner(bytes.NewReader(out))
	sc.Buffer(make([]byte, 0, 64*1024), 1024*1024)
	for sc.Scan() {
		m := objdumpLine.FindSubmatch(sc.Bytes())
		if m == nil {
			continue
		}
		asm := strings.TrimSpace(string(m[2]))
		if strings.HasPrefix(asm, "?") || asm == "INT $0x3" {
			continue // padding
		}
		n += int64(len(m[1]) / 2)
	}
	return n
}

// objdumpLine matches an instruction line of go tool objdump: file:line,
// address, the instruction's bytes in hex, and its text.
var objdumpLine = regexp.MustCompile(`^\s+\S+:\d+\s+0x[0-9a-f]+\s+([0-9a-f]+)\s+(.*)$`)

// modulePath is this repository's module path, from go.mod.
func modulePath(root string) string {
	b, err := os.ReadFile(filepath.Join(root, "go.mod"))
	if err != nil {
		return ""
	}
	for _, line := range strings.Split(string(b), "\n") {
		if f := strings.Fields(line); len(f) == 2 && f[0] == "module" {
			return f[1]
		}
	}
	return ""
}
