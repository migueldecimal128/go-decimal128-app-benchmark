# decimal64 against decimal128: microbenchmarks

Operation and memory benchmarks of Go's predeclared `decimal64` and
`decimal128`, on the same values at both widths, to set beside the
application benchmarks (`go-builtin-decimal64` in [`../results/`](../results/RESULTS.md)).

## Running

They need a toolchain with the decimal types, such as the
[ubd128 fork](https://github.com/migueldecimal128/go-decimal-ubd/tree/ubd128)
(`decimal64` stored as XOR-BID64, `decimal128` as UBD):

```
cd microbench
export PATH=~/go-decimal-ubd/bin:$PATH GOTOOLCHAIN=local GOEXPERIMENT=decimal
go test -run TestSameValues .                                         # same values at both widths
go test -run XXX -bench . -ldflags=-funcalign=64 .                    # decimal128 at -d=ubdlevel=2, the default
go test -run XXX -bench . -ldflags=-funcalign=64 -gcflags=-d=ubdlevel=3 .
```

`-d=ubdlevel` changes only `decimal128`'s code. Level 3 runs level 2's inline
paths first and, behind them, paths derived from the BID lowering, the same
kind as `decimal64`'s; level 2 sends what its paths do not take to the kernel. Rows are named `Op/Width/Class` (`BenchmarkOps`) or
`Op/Width/n=<values>` (the others).

`width64.go` and `width128.go` are generated from `width.go.tmpl` by
`go run gen.go`.

## What each benchmark measures

**`BenchmarkOps`**: one operation per iteration, over a pool of operands:

| class | operands | order |
|---|---|---|
| `money` | eight two-place amounts (1234.56, 78.90, …), each paired with the one three places on | fixed rotation, which the branch predictor learns |
| `d16` | 1,024 pairs, 1–16 digits, exponents −4 to 0, random signs | drawn at random |
| `cents` | 1,024 pairs, 3–10 digits at exponent −2, one in ten negative; rates of 1–5 digits at −4 | drawn at random |
| `cents-pos`, `cents-half` | as `cents`, with no amount negative, and half | drawn at random |
| `nearby` | money amounts plus four-place rates | fixed rotation |

The random draw costs about 1.5 ns per iteration, at both widths
(`Floor/cents` is the draw alone). Every value is exact at both widths
(`TestSameValues`), but results need not be: a product of two 16-digit
coefficients, or a sum of operands with different exponents, rounds at
`decimal64`'s 16 digits and is exact at `decimal128`'s 34 (`Mul/d16`,
`FMA/d16`, `Mul/cents`).

**`BenchmarkMem`**: loops over slices of n money values, from 1K values (8 or
16 KB) to 16M (128 or 256 MB): `Sum` (a running total), `Max`, and
`MulQuant` (`c[i] = Quantize(a[i]*q[i], 0.01)`, two slices read and one
written).

**`BenchmarkGather`**: the sum of 1M values read at random indices from a
table of n values, so that the table's size, 8n or 16n bytes, decides which
cache level holds it.

**`BenchmarkSort`**: `slices.SortFunc` over n `d16` values.

## Results

Apple M3 Pro (16 MB L2 per performance cluster), ubd128 fork at `447e9c0b4e`,
`-ldflags=-funcalign=64`, 2026-11-23. Medians of six runs for the operations,
four for the rest. The ratios are `decimal128` time over `decimal64` time:
below 1, `decimal128` is faster.

### Operations (ns per operation)

| row | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|--:|--:|--:|--:|--:|
| Add/money | 0.83 | 0.88 | 0.88 | 1.07 | 1.07 |
| Add/d16 | 9.41 | 12.17 | 6.74 | 1.29 | 0.72 |
| Add/cents | 3.28 | 4.39 | 3.23 | 1.34 | 0.98 |
| Add/cents-pos | 1.73 | 1.73 | 1.73 | 1.00 | 1.00 |
| Add/nearby | 5.80 | 4.24 | 2.22 | 0.73 | 0.38 |
| Sub/money | 1.32 | 1.21 | 1.23 | 0.91 | 0.93 |
| Sub/cents | 3.53 | 4.05 | 3.25 | 1.15 | 0.92 |
| Mul/money | 1.36 | 1.41 | 1.41 | 1.04 | 1.04 |
| Mul/d16 | 10.85 | 2.18 | 2.17 | 0.20 | 0.20 |
| Mul/cents | 4.60 | 2.18 | 2.17 | 0.47 | 0.47 |
| Div/money | 7.59 | 10.94 | 10.90 | 1.44 | 1.44 |
| Div/d16 | 10.82 | 19.05 | 18.94 | 1.76 | 1.75 |
| Div/cents | 7.62 | 14.50 | 14.52 | 1.90 | 1.90 |
| Less/money | 1.73 | 1.07 | 1.07 | 0.62 | 0.62 |
| Less/cents | 2.71 | 4.04 | 3.32 | 1.49 | 1.23 |
| Less/cents-pos | 2.70 | 1.85 | 1.87 | 0.69 | 0.69 |
| Less/cents-half | 2.70 | 7.73 | 5.79 | 2.86 | 2.14 |
| Equal/money | 1.73 | 0.80 | 0.80 | 0.46 | 0.47 |
| Equal/cents | 2.69 | 3.92 | 3.29 | 1.45 | 1.22 |
| Quantize/money | 0.94 | 1.32 | 0.91 | 1.40 | 0.97 |
| Quantize/d16 | 7.51 | 8.03 | 5.63 | 1.07 | 0.75 |
| MulQuantize/cents | 2.92 | 3.75 | 2.85 | 1.28 | 0.98 |
| MulQuantizeRate/cents | 4.75 | 4.80 | 3.79 | 1.01 | 0.80 |
| FMA/d16 | 21.25 | 14.38 | 14.21 | 0.68 | 0.67 |
| Sqrt/money | 896.55 | 896.50 | 904.05 | 1.00 | 1.01 |
| FromInt/int | 2.91 | 0.25 | 0.25 | 0.09 | 0.08 |
| Parse/d16 | 26.28 | 21.95 | 22.48 | 0.84 | 0.86 |
| Format/d16 | 12.93 | 8.48 | 8.46 | 0.66 | 0.65 |

`go test -bench Ops` prints every row, including Sub, Less and Equal for each
class and FMA on money.

### Slices (µs per pass)

| row | bytes, 64 / 128 | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|---|--:|--:|--:|--:|--:|
| Sum, 1K values | 8 / 16 KB | 0.64 | 0.48 | 0.48 | 0.75 | 0.75 |
| Sum, 1M values | 8 / 16 MB | 650 | 488 | 473 | 0.75 | 0.73 |
| Sum, 16M values | 128 / 256 MB | 10,407 | 7,593 | 7,645 | 0.73 | 0.73 |
| Max, 1K values | 8 / 16 KB | 1.36 | 1.59 | 0.82 | 1.16 | 0.60 |
| Max, 16M values | 128 / 256 MB | 22,412 | 27,219 | 13,406 | 1.21 | 0.60 |
| MulQuant, 1K values | 8 / 16 KB | 2.66 | 3.84 | 2.51 | 1.44 | 0.94 |
| MulQuant, 16M values | 128 / 256 MB | 42,534 | 67,138 | 42,616 | 1.58 | 1.00 |

Each ratio is the same from 1K values to 16M: these loops are bound by the
arithmetic, not by memory traffic, and the 8 and 16 bytes per value do not
show.

### Random lookups (µs per 1M lookups)

| table | bytes, 64 / 128 | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|---|--:|--:|--:|--:|--:|
| 4K values | 32 / 64 KB | 569 | 604 | 612 | 1.06 | 1.08 |
| 64K values | 0.5 / 1 MB | 1,036 | 889 | 887 | 0.86 | 0.86 |
| 1M values | 8 / 16 MB | 1,157 | 1,319 | 1,336 | 1.14 | 1.15 |
| 1.25M values | 10 / 20 MB | 1,177 | 1,956 | 1,974 | 1.66 | 1.68 |
| 1.5M values | 12 / 24 MB | 1,191 | 3,310 | 3,288 | 2.78 | 2.76 |
| 1.75M values | 14 / 28 MB | 1,236 | 3,773 | 3,799 | 3.05 | 3.07 |
| 2M values | 16 / 32 MB | 1,442 | 4,085 | 4,133 | 2.83 | 2.87 |
| 2.5M values | 20 / 40 MB | 2,219 | 4,398 | 4,395 | 1.98 | 1.98 |
| 3M values | 24 / 48 MB | 3,670 | 4,589 | 4,592 | 1.25 | 1.25 |
| 4M values | 32 / 64 MB | 4,727 | 4,935 | 4,934 | 1.04 | 1.04 |
| 16M values | 128 / 256 MB | 6,438 | 5,701 | 5,540 | 0.89 | 0.86 |

The difference made by the size of a value is the window where
`decimal64`'s table still fits the 16 MB L2 and `decimal128`'s does not,
about 12–20 MB of `decimal64` (1.5M–2.5M values): there `decimal64` is
2.8–3.1 times as fast as `decimal128` at either level. Outside the window the
two widths are within about 15% of each other.

### Sorting (µs per sort)

| values | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|--:|--:|--:|--:|--:|
| 1K | 49 | 90 | 45 | 1.85 | 0.91 |
| 1M | 188,390 | 242,937 | 191,022 | 1.29 | 1.01 |
