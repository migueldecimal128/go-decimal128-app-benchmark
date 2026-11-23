#!/bin/sh
# SPDX-License-Identifier: MIT
#
# run-toolchains.sh: on a new machine or architecture, build the decimal
# toolchains, run the application benchmark for every toolchain library
# (go-builtin-decimal64 and the decimal128 builds: XOR-BID, UBD levels 1, 2
# and 3), run the microbenchmarks at UBD levels 2 and 3, then commit the
# results and push them.
#
#	sh scripts/run-toolchains.sh
#
# The toolchains go in ~/go-decimal-ubd (the ubd128 fork) and
# ~/go-decimal-base (Marcelo Cantos's XOR-BID toolchain, a worktree of it at
# 5032fd7754), or wherever GO_DECIMAL_GOROOT and GO_DECIMAL_XORBID_GOROOT
# point. Each is cloned or updated and rebuilt as needed. Needs git, a C
# toolchain and Go 1.26.4 or later as go.
set -eu

cd "$(dirname "$0")/.."
UBD=${GO_DECIMAL_GOROOT:-$HOME/go-decimal-ubd}
BASE=${GO_DECIMAL_XORBID_GOROOT:-$HOME/go-decimal-base}
BASE_COMMIT=5032fd7754558a75aabd732083ef2c92bb8a0daa
export GOROOT_BOOTSTRAP="${GOROOT_BOOTSTRAP:-$(go env GOROOT)}"

# Keep a Mac awake until this script exits.
if command -v caffeinate >/dev/null 2>&1; then
	caffeinate -dims -w $$ &
fi

echo "== benchmark: update"
git pull --ff-only

echo "== ubd128 toolchain: $UBD"
if [ ! -d "$UBD" ]; then
	git clone --branch ubd128 https://github.com/migueldecimal128/go-decimal-ubd.git "$UBD"
else
	git -C "$UBD" pull --ff-only
fi
(cd "$UBD/src" && ./make.bash)

echo "== XOR-BID toolchain: $BASE"
if [ ! -d "$BASE" ]; then
	git -C "$UBD" worktree add "$BASE" "$BASE_COMMIT"
fi
if [ ! -x "$BASE/bin/go" ]; then
	(cd "$BASE/src" && ./make.bash)
fi
"$UBD/bin/go" version
"$BASE/bin/go" version

echo "== application benchmark"
export GO_DECIMAL_GOROOT="$UBD" GO_DECIMAL_XORBID_GOROOT="$BASE"
export GOFLAGS=-ldflags=-funcalign=64
go run ./cmd/bench -lib go-builtin-decimal64,go-builtin-decimal128-ubd-level1,go-builtin-decimal128,go-builtin-decimal128-ubd-level3,go-builtin-decimal128-xorbid

echo "== microbenchmarks"
arch=$(uname -m)
mkdir -p microbench/results
(
	cd microbench
	export PATH="$UBD/bin:$PATH" GOTOOLCHAIN=local GOEXPERIMENT=decimal
	unset GOFLAGS
	go test -run XXX -bench . -count 4 -benchtime 300ms -ldflags=-funcalign=64 . > "results/$arch-level2.txt"
	go test -run XXX -bench . -count 4 -benchtime 300ms -ldflags=-funcalign=64 -gcflags=-d=ubdlevel=3 . > "results/$arch-level3.txt"
)

echo "== commit and push"
ubd=$(git -C "$UBD" rev-parse --short=10 HEAD)
git add results "microbench/results/$arch-level2.txt" "microbench/results/$arch-level3.txt"
git commit -m "Results: $arch run, toolchain libraries at $ubd" \
	-m "go-builtin-decimal64, go-builtin-decimal128-ubd-level1, go-builtin-decimal128, go-builtin-decimal128-ubd-level3 and go-builtin-decimal128-xorbid, and the microbenchmarks at UBD levels 2 and 3 (microbench/results/$arch-level*.txt), by scripts/run-toolchains.sh."
if git push; then
	echo "== done: results pushed"
else
	echo "== results are committed here but the push failed; push them with: git push"
fi
