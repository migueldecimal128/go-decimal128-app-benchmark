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

// notInlined lists the wrappers that exceed Go's inlining budget (80), each
// with where it runs and what that costs. Any other wrapper must inline.
var notInlined = map[string]string{
	// miguel-decimal128-go: cost 81. Used only to decode input: risk while preloading
	// its series, tax while capturing its rows, banking at open and in
	// prepare(), euro in Decode() before the clock starts. No timed cost.
	"FromBID128": "setup only",
	// miguel-decimal128-go: cost 82 (its own 3-argument method already costs 77).
	// Telco truncates its taxes with it, 1-2 calls per call record, inside the
	// timed loop: measured +0.9% fastest sample, +1.2% median (about 0.8 ns
	// per call record, alternating A/B against direct library calls).
	"Rounding.Quantize": "timed: telco +0.9% / +1.2%",
}

// TestWrappersInline checks that the compiler inlines every wrapper in this
// library's adapter, so calling a library through it costs nothing extra.
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
		switch {
		case !canInline[name] && !exempt:
			t.Errorf("%s does not inline: every call through it would add a function call", name)
		case canInline[name] && exempt:
			t.Logf("%s now inlines; it can come off the notInlined list", name)
		}
	}
}
