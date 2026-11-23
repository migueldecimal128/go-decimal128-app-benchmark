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

Intel(R) Core(TM) i9-9880H CPU @ 2.30GHz · go1.26.4 · all rows measured in one run, 2026-11-23 20:27 UTC · benchmark code `d5852f4 + uncommitted changes`

| program | decimal64-XOR-BID `37b9ac04c0` | decimal128-XOR-BID `5032fd7754` | decimal128-UBD level 2 `37b9ac04c0` | decimal128-UBD level 3 `37b9ac04c0` | vs decimal128-XOR-BID | vs decimal128-UBD level 2 | vs decimal128-UBD level 3 |
|---|--:|--:|--:|--:|--:|--:|--:|
| euro_conversion | 1,643.2 | 1,949.0 | 1,550.3 | 1,522.2 | −16% | +6% | +8% |
| risk_management | 14,488.4 | 15,318.3 | 15,051.0 | 14,670.8 | −5% | −4% | −1% |
| tax_preparation fed | 5,479.0 | 5,063.1 | 4,426.0 | 4,124.6 | +8% | +24% | +33% |
| tax_preparation ca | 1,250.6 | 1,120.7 | 1,016.5 | 1,010.6 | +12% | +23% | +24% |
| tax_preparation nc | 1,879.5 | 1,741.5 | 1,502.6 | 1,434.9 | +8% | +25% | +31% |
| banking | 1,369.3 | 1,451.4 | 1,366.5 | 1,345.0 | −6% | 0% | +2% |
| telco | 85.3 | 78.1 | 65.9 | 66.3 | +9% | +29% | +29% |
| geometric mean | | | | | **+1.0%** | **+14.1%** | **+17.0%** |

