# go-decimal128-app-benchmark

Go implementations of the UW-Madison decimal floating-point application
benchmarks (euro conversion, risk management, tax preparation, banking) and of
Mike Cowlishaw's telco benchmark, for measuring Go decimal128 libraries on
whole applications rather than single operations.

Each program reproduces the reference output **byte for byte** (the UW-Madison
apps: the original C program's output, produced with decNumber; telco: its
1,000,000-line output and Cowlishaw's published control totals), so a timing
only counts when the output is right.

## Status

- **Apps:** copied from
  [decimal128-app-benchmark](https://github.com/migueldecimal128/decimal128-app-benchmark)
  (commit `da7019d`), unchanged apart from module and import paths. All pass
  their gates: euro, banking and tax (fed/ca/nc) byte-exact, risk 20/20.
- **Library adapter:** the apps reach a decimal library only through
  [`internal/dec`](internal/dec/doc.go), the Go counterpart of the C suite's
  `operator.h`. The library is chosen at compile time by a build tag, with no
  interface or dispatch on the measured path. Its documentation states the
  contract every library must meet and what the layer costs.
- **Libraries:** [decimal128-go](https://github.com/migueldecimal128/decimal128-go).
- **Telco:** added after the copy, based on decimal128-go's own telco program.
- **Data:** inputs and goldens for all five apps are in [`data/`](data/README.md)
  (9.3 MB, with provenance and SHA-256 checksums).
- **Runner:** `go run ./cmd/bench` builds, gates and times every app; results
  are in [`results/RESULTS.md`](results/RESULTS.md).
- **Next:** other Go decimal128 libraries.

Code comments still name paths in decimal128-app-benchmark (for example
`apps/<app>/c/src/main.c`); those refer to that repository.

## Layout

```
apps/euro_conversion/cmd/euro/           euro conversion
apps/risk_management/cmd/risk/           risk management (CAPM)
apps/tax_preparation/cmd/tax-{fed,ca,nc}/
apps/tax_preparation/internal/tax/       the three tax forms
apps/banking/cmd/banking/                banking
apps/telco/cmd/telco/                    telco (MIT)
internal/dec/                            the library adapter (MIT)
internal/harness/                        shared timing and I/O helpers (MIT)
cmd/bench/                               the runner (MIT)
baseline/noop/                           startup baseline program (MIT)
results/                                 records and RESULTS.md
data/<app>/                              inputs and goldens (see data/README.md)
```

## Running

Requires Go 1.26.4 or later. From the repository root:

```
go run ./cmd/bench                 # build, gate and time every app
go run ./cmd/bench -app banking    # one app
go run ./cmd/bench -lib decimal128-go   # one library
go run ./cmd/bench -report-only    # rewrite results/RESULTS.md from the records
```

Each run appends one record per row to `results/data/<app>.jsonl` and rewrites
`results/RESULTS.md`, which explains its columns. In short:

- **Gate:** every run's output is compared byte for byte with the golden
  (risk: each stock's output file); a mismatch marks the row ✗.
- **compute:** the program's own timed loop. The runner sizes it so each timed
  sample lasts at least 100 µs, with about 200 ms of samples, runs it in 3
  processes, and cross-checks it against its own clock.
- **whole:** one run from process start to exit, fastest of 9; **startup** is a
  do-nothing Go program launched the same way.

To build the programs only: `go build -trimpath -o bin/ ./apps/...` (add
`-tags <library>` for a library other than the default).

## Adding a library

1. Add its adapter file to `internal/dec` (see the package documentation).
2. `go test -tags <library> ./internal/dec` checks the contract, that every
   wrapper inlines, and that no app bypasses the adapter.
3. Add it to the `libraries` table in `cmd/bench/main.go` and run the
   benchmark; every run is gated against the goldens.

The library must have a GPLv3-compatible license (see below).

## Licensing

This repository is **GPL-3.0-or-later** because four of the applications are
translations of the GPL-licensed UW-Madison suite (tax preparation also derives
from OpenTaxSolver). Telco is Mike Cowlishaw's, freely distributable, and its
program here is MIT. See [NOTICE](NOTICE) for attribution.

- **The libraries it measures keep their own licenses.** decimal128-go (MIT) is
  an ordinary Go module dependency pinned in `go.mod`; it is fetched by the Go
  toolchain and not copied here.
- **Running the benchmarks creates no obligations.** GPL obligations apply only
  when the programs are distributed, for example as built binaries; a
  distributed binary must come with its complete source and must keep each
  library's license notice.
- **Code flows one way.** Nothing from this repository goes into the libraries
  it measures; library changes are made in the library's own repository.
- **`internal/dec`, `internal/harness`, the runner and the baseline are MIT.**
  They are benchmark plumbing, not derived from the UW-Madison sources.
- **Any library added later must be GPLv3-compatible** (MIT, BSD and Apache-2.0
  are).
