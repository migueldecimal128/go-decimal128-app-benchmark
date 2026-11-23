# decimal64 and decimal128

Generated with [RESULTS.md](RESULTS.md), from the same records: each program's **compute** time, ns per record, with Go's built-in `decimal64` (go-builtin-decimal64) and with three builds of `decimal128`: Marcelo Cantos's toolchain (XOR-BID, the baseline of RESULTS.md) and the ubd128 fork at its default inline level (2) and at level 3, which adds the kernel's own first paths inline behind level 2's (since the fork's 0493529123; in runs at 447e9c0b4e and before, level 3 was the inline paths derived from the BID lowering, as `decimal64`'s are). `decimal64` and UBD come from the same toolchain, the ubd128 fork, where `decimal64` is stored as XOR-BID64 (the decimal64-XOR-BID column).

Each program is the same source at both widths; at `decimal64` it reads the UW-Madison suite's 64-bit BID inputs, and every row passed its gate against the suite's 64-bit golden or, where `decimal64`'s output is the same, the 128-bit one (see [data/README.md](../data/README.md)). A percentage is `decimal64`'s time against that column's: + means `decimal64` is slower, − faster. Operation and memory microbenchmarks are in [microbench/README.md](../microbench/README.md).

## arm64

Apple M3 Pro · go1.26.4 · all rows measured in one run, 2026-11-23 20:00 UTC · benchmark code `c7bbd15`

| program | decimal64-XOR-BID `37b9ac04c0` | decimal128-XOR-BID `5032fd7754` | decimal128-UBD level 2 `37b9ac04c0` | decimal128-UBD level 3 `37b9ac04c0` | vs decimal128-XOR-BID | vs decimal128-UBD level 2 | vs decimal128-UBD level 3 |
|---|--:|--:|--:|--:|--:|--:|--:|
| euro_conversion | 744.4 | 881.0 | 723.8 | 705.8 | −16% | +3% | +5% |
| risk_management | 5,400.0 | 5,789.6 | 5,593.8 | 5,418.8 | −7% | −3% | 0% |
| tax_preparation fed | 2,661.2 | 2,345.0 | 2,229.6 | 2,055.3 | +13% | +19% | +29% |
| tax_preparation ca | 568.0 | 500.0 | 473.8 | 437.3 | +14% | +20% | +30% |
| tax_preparation nc | 881.9 | 801.3 | 695.4 | 663.9 | +10% | +27% | +33% |
| banking | 631.6 | 648.2 | 632.5 | 618.3 | −3% | 0% | +2% |
| telco | 33.1 | 32.0 | 25.9 | 25.6 | +3% | +28% | +29% |
| geometric mean | | | | | **+1.7%** | **+12.6%** | **+17.6%** |

## x86_64

Intel(R) Core(TM) i9-9880H CPU @ 2.30GHz · go1.26.4 · all rows measured in one run, 2026-11-23 03:07 UTC · benchmark code `73c4723 + uncommitted changes`

| program | decimal64-XOR-BID `447e9c0b4e` | decimal128-XOR-BID `5032fd7754` | decimal128-UBD level 2 `447e9c0b4e` | decimal128-UBD level 3 `447e9c0b4e` | vs decimal128-XOR-BID | vs decimal128-UBD level 2 | vs decimal128-UBD level 3 |
|---|--:|--:|--:|--:|--:|--:|--:|
| euro_conversion | 3,429.8 | 3,443.2 | 3,309.8 | 3,288.1 | 0% | +4% | +4% |
| risk_management | 25,705.9 | 27,072.6 | 29,374.0 | 26,633.8 | −5% | −12% | −3% |
| tax_preparation fed | 9,882.5 | 8,963.7 | 8,677.9 | 7,961.9 | +10% | +14% | +24% |
| tax_preparation ca | 2,263.2 | 1,970.0 | 1,987.1 | 1,815.9 | +15% | +14% | +25% |
| tax_preparation nc | 3,525.2 | 3,076.1 | 3,036.0 | 2,864.4 | +15% | +16% | +23% |
| banking | 2,462.8 | 2,509.7 | 2,490.8 | 2,424.0 | −2% | −1% | +2% |
| telco | 152.7 | 137.3 | 133.2 | 122.0 | +11% | +15% | +25% |
| geometric mean | | | | | **+6.0%** | **+6.4%** | **+13.6%** |

