# go-decimal128-app-benchmark

Go implementations of the four UW-Madison decimal floating-point application
benchmarks (euro conversion, risk management, tax preparation, banking), for
measuring Go decimal128 libraries on whole applications rather than single
operations.

Each program reproduces the original C program's output **byte for byte**, so a
timing only counts when the output matches the reference ("golden") output
produced with decNumber.

## Status

- **Apps:** copied from
  [decimal128-app-benchmark](https://github.com/migueldecimal128/decimal128-app-benchmark)
  (commit `da7019d`), unchanged apart from module and import paths. All pass
  their gates: euro, banking and tax (fed/ca/nc) byte-exact, risk 20/20.
- **Library:** [decimal128-go](https://github.com/migueldecimal128/decimal128-go)
  only, called directly.
- **Data:** inputs and goldens for all four apps are in [`data/`](data/README.md)
  (1.7 MB, with provenance and SHA-256 checksums).
- **Next:** a Go-native runner (`go run ./cmd/bench`) that builds, gates and
  times the apps; then a compile-time library adapter so other Go decimal
  libraries can be measured.

Code comments still name paths in decimal128-app-benchmark (for example
`apps/<app>/c/src/main.c`); those refer to that repository.

## Layout

```
apps/euro_conversion/cmd/euro-d128/      euro conversion
apps/risk_management/cmd/risk-d128/      risk management (CAPM)
apps/tax_preparation/cmd/tax-{fed,ca,nc}-d128/
apps/tax_preparation/internal/tax/       the three tax forms
apps/banking/cmd/banking-d128/           banking
internal/harness/                        shared timing and I/O helpers (MIT)
data/<app>/                              inputs and goldens (see data/README.md)
```

## Building

Requires Go 1.26.4 or later.

```
go build -trimpath -o bin/ ./...
```

Until the runner exists, an app can be checked by hand from its data folder,
for example euro conversion:

```
cd data/euro_conversion
../../bin/euro-d128 testGen/bin/Euro.bin.BID128 euro_run.out 2>/tmp/euro.err
cmp /tmp/euro.err golden/Euro.output.DECNUMBER_FIXED128 && echo OK
```

## Licensing

This repository is **GPL-3.0-or-later** because the four applications are
translations of the GPL-licensed UW-Madison suite (tax preparation also derives
from OpenTaxSolver). See [NOTICE](NOTICE) for attribution.

- **The libraries it measures keep their own licenses.** decimal128-go (MIT) is
  an ordinary Go module dependency pinned in `go.mod`; it is fetched by the Go
  toolchain and not copied here.
- **Running the benchmarks creates no obligations.** GPL obligations apply only
  when the programs are distributed, for example as built binaries; a
  distributed binary must come with its complete source and must keep each
  library's license notice.
- **Code flows one way.** Nothing from this repository goes into the libraries
  it measures; library changes are made in the library's own repository.
- **`internal/harness` is MIT.** It is benchmark plumbing, not derived from the
  UW-Madison sources.
- **Any library added later must be GPLv3-compatible** (MIT, BSD and Apache-2.0
  are).
