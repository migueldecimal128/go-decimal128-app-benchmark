// SPDX-License-Identifier: MIT

package dec

import (
	"go/ast"
	"go/build"
	"go/parser"
	"go/token"
	"os/exec"
	"path/filepath"
	"regexp"
	"testing"
)

// TestWrappersInline checks that the compiler inlines every wrapper in this
// library's adapter, so calling a library through it costs nothing extra.
// Exceptions are listed, with their cost, in the library file's notInlined.
func TestWrappersInline(t *testing.T) {
	if _, err := exec.LookPath("go"); err != nil {
		t.Skip("go command not found")
	}
	// The exported functions and methods of this build's adapter files.
	ctx := build.Default
	if BuildTag != "" {
		ctx.BuildTags = []string{BuildTag}
	}
	pkg, err := ctx.ImportDir(".", 0)
	if err != nil {
		t.Fatal(err)
	}
	want := map[string]bool{}
	fset := token.NewFileSet()
	for _, name := range pkg.GoFiles {
		f, err := parser.ParseFile(fset, filepath.Join(pkg.Dir, name), nil, 0)
		if err != nil {
			t.Fatal(err)
		}
		for _, decl := range f.Decls {
			fd, ok := decl.(*ast.FuncDecl)
			if !ok || !fd.Name.IsExported() {
				continue
			}
			full := fd.Name.Name
			if fd.Recv != nil {
				full = fd.Recv.List[0].Type.(*ast.Ident).Name + "." + full
			}
			want[full] = true
		}
	}
	if len(want) == 0 {
		t.Fatal("no exported wrappers found")
	}

	args := []string{"build", "-gcflags=-m", "-o", "/dev/null"}
	if BuildTag != "" {
		args = append(args, "-tags", BuildTag)
	}
	out, err := exec.Command("go", append(args, ".")...).CombinedOutput()
	if err != nil {
		t.Fatalf("go build: %v\n%s", err, out)
	}
	canInline := map[string]bool{}
	re := regexp.MustCompile(`: can inline (\S+)`)
	for _, m := range re.FindAllStringSubmatch(string(out), -1) {
		canInline[m[1]] = true
	}
	for name := range want {
		_, exempt := notInlined[name]
		if _, ok := unsupported[name]; ok {
			continue // a stub that panics; never called by a supported app
		}
		switch {
		case !canInline[name] && !exempt:
			t.Errorf("%s does not inline: every call through it would add a function call", name)
		case canInline[name] && exempt:
			t.Logf("%s now inlines; it can come off the notInlined list", name)
		}
	}
}
