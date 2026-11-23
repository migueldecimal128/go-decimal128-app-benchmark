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

`-d=ubdlevel` changes only `decimal128`'s code. Level 3 gives it inline paths
derived from the BID lowering, the same kind as `decimal64`'s; level 2 inlines
less. Rows are named `Op/Width/Class` (`BenchmarkOps`) or
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

Apple M3 Pro (16 MB L2 per performance cluster), ubd128 fork at `04befab964`,
`-ldflags=-funcalign=64`, 2026-11-23. Medians of six runs for the operations,
four for the rest. The ratios are `decimal128` time over `decimal64` time:
below 1, `decimal128` is faster.

### Operations (ns per operation)

| row | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|--:|--:|--:|--:|--:|
| Add/money | 0.83 | 0.88 | 1.01 | 1.07 | 1.22 |
| Add/d16 | 9.40 | 12.21 | 7.80 | 1.30 | 0.83 |
| Add/cents | 3.27 | 4.40 | 4.43 | 1.34 | 1.36 |
| Add/cents-pos | 1.73 | 1.73 | 1.81 | 1.00 | 1.04 |
| Add/nearby | 5.81 | 4.24 | 2.09 | 0.73 | 0.36 |
| Sub/money | 1.32 | 1.21 | 1.50 | 0.91 | 1.13 |
| Sub/cents | 3.54 | 4.06 | 4.56 | 1.15 | 1.29 |
| Mul/money | 1.36 | 1.41 | 1.30 | 1.04 | 0.96 |
| Mul/d16 | 10.86 | 2.18 | 3.44 | 0.20 | 0.32 |
| Mul/cents | 4.62 | 2.17 | 3.31 | 0.47 | 0.72 |
| Div/money | 7.60 | 10.93 | 10.91 | 1.44 | 1.44 |
| Div/d16 | 10.84 | 19.09 | 19.06 | 1.76 | 1.76 |
| Div/cents | 7.63 | 14.54 | 14.58 | 1.91 | 1.91 |
| Less/money | 1.73 | 1.07 | 1.91 | 0.62 | 1.10 |
| Less/cents | 2.71 | 4.05 | 3.45 | 1.50 | 1.27 |
| Less/cents-pos | 2.70 | 1.86 | 2.57 | 0.69 | 0.95 |
| Less/cents-half | 2.71 | 7.74 | 3.45 | 2.86 | 1.27 |
| Equal/money | 1.73 | 0.80 | 1.91 | 0.46 | 1.10 |
| Equal/cents | 2.70 | 3.92 | 3.43 | 1.45 | 1.27 |
| Quantize/money | 0.94 | 1.33 | 0.91 | 1.41 | 0.97 |
| Quantize/d16 | 7.53 | 8.04 | 5.69 | 1.07 | 0.76 |
| MulQuantize/cents | 2.92 | 3.75 | 2.71 | 1.28 | 0.93 |
| MulQuantizeRate/cents | 4.76 | 4.79 | 3.71 | 1.01 | 0.78 |
| FMA/d16 | 21.30 | 14.40 | 14.21 | 0.68 | 0.67 |
| Sqrt/money | 893.75 | 899.65 | 901.25 | 1.01 | 1.01 |
| FromInt/int | 2.91 | 0.25 | 0.25 | 0.09 | 0.08 |
| Parse/d16 | 26.42 | 21.95 | 22.54 | 0.83 | 0.85 |
| Format/d16 | 12.88 | 8.47 | 8.57 | 0.66 | 0.67 |

`go test -bench Ops` prints every row, including Sub, Less and Equal for each
class and FMA on money.

### Slices (µs per pass)

| row | bytes, 64 / 128 | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|---|--:|--:|--:|--:|--:|
| Sum, 1K values | 8 / 16 KB | 0.64 | 0.48 | 0.64 | 0.75 | 1.01 |
| Sum, 1M values | 8 / 16 MB | 651 | 482 | 661 | 0.74 | 1.02 |
| Sum, 16M values | 128 / 256 MB | 10,407 | 7,610 | 10,583 | 0.73 | 1.02 |
| Max, 1K values | 8 / 16 KB | 1.36 | 1.59 | 1.25 | 1.16 | 0.91 |
| Max, 16M values | 128 / 256 MB | 22,412 | 27,836 | 21,091 | 1.24 | 0.94 |
| MulQuant, 1K values | 8 / 16 KB | 2.67 | 3.84 | 2.44 | 1.44 | 0.92 |
| MulQuant, 16M values | 128 / 256 MB | 42,572 | 67,135 | 41,739 | 1.58 | 0.98 |

Each ratio is the same from 1K values to 16M: these loops are bound by the
arithmetic, not by memory traffic, and the 8 and 16 bytes per value do not
show.

### Random lookups (µs per 1M lookups)

| table | bytes, 64 / 128 | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|---|--:|--:|--:|--:|--:|
| 4K values | 32 / 64 KB | 564 | 607 | 717 | 1.08 | 1.27 |
| 64K values | 0.5 / 1 MB | 1,036 | 886 | 1,208 | 0.86 | 1.17 |
| 1M values | 8 / 16 MB | 1,158 | 1,328 | 1,656 | 1.15 | 1.43 |
| 1.25M values | 10 / 20 MB | 1,173 | 1,856 | 2,413 | 1.58 | 2.06 |
| 1.5M values | 12 / 24 MB | 1,188 | 3,277 | 4,315 | 2.76 | 3.63 |
| 1.75M values | 14 / 28 MB | 1,248 | 3,800 | 5,186 | 3.05 | 4.16 |
| 2M values | 16 / 32 MB | 1,484 | 4,108 | 5,880 | 2.77 | 3.96 |
| 2.5M values | 20 / 40 MB | 2,031 | 4,397 | 6,602 | 2.17 | 3.25 |
| 3M values | 24 / 48 MB | 3,591 | 4,584 | 6,861 | 1.28 | 1.91 |
| 4M values | 32 / 64 MB | 4,748 | 4,955 | 7,414 | 1.04 | 1.56 |
| 16M values | 128 / 256 MB | 6,209 | 5,534 | 8,306 | 0.89 | 1.34 |

The difference made by the size of a value is the window where
`decimal64`'s table still fits the 16 MB L2 and `decimal128`'s does not,
about 12–20 MB of `decimal64` (1.5M–2.5M values): there `decimal64` is
2.8–3.1 times as fast as `decimal128` at level 2, and 3.6–4.2 times at level 3. Outside the window
the ratio is that of the adds alone: about 1 at level 2, and 1.2–1.6 at level
3, whose add in this loop is slower (the same as `Sum` above).

### Sorting (µs per sort)

| values | `decimal64` | `decimal128` L2 | `decimal128` L3 | L2 / 64 | L3 / 64 |
|---|--:|--:|--:|--:|--:|
| 1K | 48 | 91 | 45 | 1.90 | 0.95 |
| 1M | 188,843 | 243,072 | 193,048 | 1.29 | 1.02 |
