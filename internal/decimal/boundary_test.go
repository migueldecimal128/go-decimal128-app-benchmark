// SPDX-License-Identifier: MIT

package decimal

import (
	"go/parser"
	"go/token"
	"io/fs"
	"path/filepath"
	"strconv"
	"strings"
	"testing"
)

const modulePath = "github.com/migueldecimal128/go-decimal128-app-benchmark"

// TestAppsUseOnlyTheAdapter checks that no app imports a decimal library (or
// any other third-party package) directly: everything outside the standard
// library must come from this module, so the library is always the one the
// build tag selects.
func TestAppsUseOnlyTheAdapter(t *testing.T) {
	root := filepath.Join("..", "..", "apps")
	n := 0
	err := filepath.WalkDir(root, func(path string, e fs.DirEntry, err error) error {
		if err != nil || e.IsDir() || !strings.HasSuffix(path, ".go") {
			return err
		}
		f, err := parser.ParseFile(token.NewFileSet(), path, nil, parser.ImportsOnly)
		if err != nil {
			return err
		}
		n++
		for _, imp := range f.Imports {
			ip, _ := strconv.Unquote(imp.Path.Value)
			first := strings.SplitN(ip, "/", 2)[0]
			if strings.Contains(first, ".") && !strings.HasPrefix(ip, modulePath+"/") {
				t.Errorf("%s imports %s; apps must reach a decimal library only through internal/decimal", path, ip)
			}
		}
		return nil
	})
	if err != nil {
		t.Fatal(err)
	}
	if n == 0 {
		t.Fatal("no app sources found")
	}
}
