package debounce

import (
	"sync"
	"sync/atomic"
	"testing"
	"time"
)

func TestDebounceConcurrentAdd(t *testing.T) {
	var wg sync.WaitGroup
	var v int32

	fn := func() {
		atomic.AddInt32(&v, 1)
	}

	debounced := New(100 * time.Millisecond)
	for i := 0; i < 50; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			debounced(fn)
		}()
	}
	wg.Wait()

	time.Sleep(1 * time.Second)

	if atomic.LoadInt32(&v) != 1 {
		t.Errorf("debounce can not working: %d", v)
	}
}

func BenchmarkDebounce(b *testing.B) {
	var v int32

	fn := func() {
		atomic.AddInt32(&v, 1)
	}

	debounced := New(100 * time.Millisecond)

	b.ResetTimer()
	b.ReportAllocs()
	for i := 0; i < b.N; i++ {
		debounced(fn)
	}

	b.StopTimer()
	time.Sleep(1 * time.Second)

	if atomic.LoadInt32(&v) != 1 {
		b.Errorf("expected value is 1, but %d", v)
	}
}
