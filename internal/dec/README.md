# internal/dec: the decimal library adapter

Every app reaches a decimal library only through this package (the Go
counterpart of the C suite's `operator.h`). Each library has one file here,
`lib_<name>.go`, and a Go build tag chooses which file is compiled; the apps
never change. The package documentation ([`doc.go`](doc.go)) states the
contract every library must meet and what the layer costs.

## Libraries

| name | file | build tag | module |
|---|---|---|---|
| miguel-decimal128-go | `lib_miguel_decimal128_go.go` | `miguel_decimal128_go` | `github.com/migueldecimal128/decimal128-go` |

There is no default library. A build without a library tag compiles
`nolib.go`, which fails with
`undefined: noLibrarySelected_buildWithTags_miguel_decimal128_go`. (Build tags
cannot contain hyphens, so a library's tag is its name with underscores.)

The name ties everything together: the adapter's `Name` constant, the
runner's `libraries` table (`cmd/bench/main.go`), the folder the programs are
built into (`results/.build/<name>/`), the `impl` field of every result
record, and the library column of `results/RESULTS.md`.

Every program prints `LIBRARY=<name>` (the `Name` it was compiled with) as its
first line. After building, the runner launches each program once and stops if
that name differs from the library it built for. A misspelled tag already fails
to compile; this catches a table entry carrying another library's tag, which
would otherwise record that library's results under the wrong name.

## Running

```
go run ./cmd/bench -lib all                 # every library, runs interleaved
go run ./cmd/bench -lib <name>[,<name>...]  # these libraries
go run ./cmd/bench -app banking -lib <name> # one app, one library

go test -tags <tag> ./internal/dec          # contract and inlining tests
go build -tags <tag> -o bin/<name>/ ./apps/...
```

With several libraries, the runner builds the apps once per library and
interleaves their runs (one process at a time, library by library), so every
library is measured under the same machine conditions and lands in the same
table.

## Adding a library

1. Add `lib_<name>.go`, starting with `//go:build <tag>`, defining every name
   the contract lists, with `Name = "<name>"`, `Module` and `BuildTag = "<tag>"`.
2. Add `!<tag>` to the `//go:build` line of `nolib.go`, so it compiles only
   when no library tag is given.
3. Add the module to `go.mod` (`go get <module>@<version>`).
4. `go test -tags <tag> ./internal/dec` checks the contract, that every
   wrapper inlines (or is listed, with its cost, in `inline_test.go`), and
   that no app bypasses the adapter.
5. Add `{name, tag, module}` to the runner's `libraries` table and run the
   benchmark; every run is gated against the goldens.

The library's license must be GPLv3-compatible (MIT, BSD and Apache-2.0 are).
