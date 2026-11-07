# The original UW-Madison suite

The University of Wisconsin-Madison Decimal Floating-Point Benchmark suite v1.0
(MESA Lab, GPLv3), kept here so anyone can check the Go programs in `apps/`
against the C programs they translate.

- **`DFPBenchmarksv1.0.gz`** is the suite as distributed (a tar.gz archive dated
  September 2009), complete and unchanged.
- **`DFPBenchmarksv1.0/`** holds the parts worth reading, extracted from that
  archive and unchanged: the C sources and headers, makefiles, build and
  test-generator scripts, and READMEs (191 files).
- **The two papers** describe the suite and its results (IEEE ICCD 2007 and
  2009).

To keep the repository small, the extracted folder leaves out what is not
source: the test data (`bin/`, `ascii/`, `expected_output/`, `dataset/`,
`Excel/` folders), three compiled test-generator executables
(`tax_preparation/testGen/TestGen_*`) and CVS metadata. All of it is in the
archive:

```
tar xzf DFPBenchmarksv1.0.gz
```

The benchmark itself reads nothing in this folder. The inputs and goldens it
uses are in [`../data/`](../data/README.md); the inputs there are identical to
the archive's files.

| Go program | C original |
|---|---|
| `apps/euro_conversion` | `DFPBenchmarksv1.0/euro_conversion` |
| `apps/risk_management` | `DFPBenchmarksv1.0/risk_management` |
| `apps/tax_preparation` | `DFPBenchmarksv1.0/tax_preparation` (fed / CA / NC solvers) |
| `apps/banking` | `DFPBenchmarksv1.0/banking` |

`DFPBenchmarksv1.0/telco` is the suite's modified version of Mike Cowlishaw's
telco benchmark; `apps/telco` follows Cowlishaw's original instead.
