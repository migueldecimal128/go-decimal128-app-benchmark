# decimal64 and decimal128

Generated with [RESULTS.md](RESULTS.md), from the same records: each program's **compute** time, ns per record, with Go's built-in `decimal64` (go-builtin-decimal64) and with three builds of `decimal128`: Marcelo Cantos's toolchain (XOR-BID, the baseline of RESULTS.md) and the ubd128 fork at its default inline level (2) and at level 3, whose inline paths are derived from the BID lowering, as `decimal64`'s are. `decimal64` and UBD come from the same toolchain, the ubd128 fork, where `decimal64` is stored as XOR-BID64 (the decimal64-XOR-BID column).

Each program is the same source at both widths; at `decimal64` it reads the UW-Madison suite's 64-bit BID inputs, and every row passed its gate against the suite's 64-bit golden or, where `decimal64`'s output is the same, the 128-bit one (see [data/README.md](../data/README.md)). A percentage is `decimal64`'s time against that column's: + means `decimal64` is slower, − faster. Operation and memory microbenchmarks are in [microbench/README.md](../microbench/README.md).

## arm64

Apple M3 Pro · go1.26.4 · all rows measured in one run, 2026-11-23 00:46 UTC · benchmark code `37ea7ff`

| program | decimal64-XOR-BID `04befab964` | decimal128-XOR-BID `5032fd7754` | decimal128-UBD level 2 `04befab964` | decimal128-UBD level 3 `04befab964` | vs decimal128-XOR-BID | vs decimal128-UBD level 2 | vs decimal128-UBD level 3 |
|---|--:|--:|--:|--:|--:|--:|--:|
| euro_conversion | 875.8 | 887.5 | 870.6 | 864.2 | −1% | +1% | +1% |
| risk_management | 5,525.0 | 5,802.1 | 6,460.4 | 5,741.6 | −5% | −14% | −4% |
| tax_preparation fed | 2,736.5 | 2,363.8 | 2,417.7 | 2,185.7 | +16% | +13% | +25% |
| tax_preparation ca | 580.6 | 502.3 | 515.1 | 454.9 | +16% | +13% | +28% |
| tax_preparation nc | 926.5 | 803.0 | 768.3 | 717.6 | +15% | +21% | +29% |
| banking | 671.3 | 680.5 | 670.3 | 647.2 | −1% | 0% | +4% |
| telco | 35.3 | 32.1 | 31.9 | 28.9 | +10% | +11% | +22% |
| geometric mean | | | | | **+6.7%** | **+5.6%** | **+14.3%** |

