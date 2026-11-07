# internal/decimal: the decimal library adapter

Every app reaches a decimal library only through this package (the Go
counterpart of the C suite's `operator.h`). Each library has one file here,
`lib_<name>.go`, and a Go build tag chooses which file is compiled; the apps
never change. The package documentation ([`doc.go`](doc.go)) states the
contract every library must meet and what the layer costs.

## Libraries

| name | file | build tag | module |
|---|---|---|---|
| miguel-decimal128-go | `lib_miguel_decimal128_go.go` | `miguel_decimal128_go` | `github.com/migueldecimal128/decimal128-go` |
| bid754 | `lib_bid754.go` | `bid754` | `github.com/sky1core/bid754/bid754-go` |

Each library file also declares three lists, which the tests and the runner use:

- **`unsupported`**: operations the library lacks. They still exist (so every
  app compiles) but panic with "<library> does not support ...". The contract
  tests skip them, and the runner's table lists the apps that need them, which
  it records as "not supported" instead of running. bid754 has no power,
  logarithm, compound or directed quantize, so it runs euro and the three tax
  forms but not risk, banking or telco.
- **`deviations`**: contract properties the library does not meet, each with
  why the apps it runs cannot reach them (bid754: Minimum/Maximum with a NaN
  operand, and a Go zero value of 0E-6176 instead of +0E+0).
- **`notInlined`**: wrappers over Go's inlining budget, with their measured
  cost.

There is no default library. A build without a library tag compiles
`nolib.go`, which fails with
`undefined: noLibrarySelected_buildWithTags_miguel_decimal128_go_or_bid754` (the name lists the tags). (Build tags
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

go test -tags <tag> ./internal/decimal          # contract and inlining tests
go build -tags <tag> -o bin/<name>/ ./apps/...
```

With several libraries, the runner builds the apps once per library and
interleaves their runs (one process at a time, library by library), so every
library is measured under the same machine conditions and lands in the same
table.

## Adding a library

1. Add `lib_<name>.go`, starting with `//go:build <tag>`, defining every name
   the contract lists, with `Name = "<name>"`, `Module` and `BuildTag = "<tag>"`,
   and its `unsupported`, `deviations` and `notInlined` lists.
2. Add `!<tag>` to the `//go:build` line of `nolib.go`, so it compiles only
   when no library tag is given.
3. Add the module to `go.mod` (`go get <module>@<version>`).
4. `go test -tags <tag> ./internal/decimal` checks the contract, that every
   wrapper inlines (or is listed, with its cost, in `inline_test.go`), and
   that no app bypasses the adapter.
5. Add `{name, tag, module, unsupported}` to the runner's `libraries` table
   (`unsupported` maps each app the library cannot run to why) and run the
   benchmark; every run is gated against the goldens.

The library's license must be GPLv3-compatible (MIT, BSD and Apache-2.0 are).
