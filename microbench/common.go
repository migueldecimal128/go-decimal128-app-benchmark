// SPDX-License-Identifier: MIT

//go:build goexperiment.decimal && go1.28

package microbench

var (
	sinkB bool
	sinkI int
)

// pow10[i] is 10^i.
var pow10 = func() (p [19]int64) {
	p[0] = 1
	for i := 1; i < 19; i++ {
		p[i] = p[i-1] * 10
	}
	return
}()

func abs(x int64) int64 {
	if x < 0 {
		return -x
	}
	return x
}
