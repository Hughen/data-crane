package debounce

import (
	"sync"
	"time"
)

type debouncer struct {
	mu    sync.Mutex
	after time.Duration
	timer *time.Timer
}

func New(after time.Duration) func(func()) {
	d := &debouncer{after: after}

	return func(fn func()) {
		d.add(fn)
	}
}

func (d *debouncer) add(fn func()) {
	d.mu.Lock()
	defer d.mu.Unlock()

	if d.timer != nil {
		if !d.timer.Stop() {
			<-d.timer.C
		}
	}

	d.timer = time.AfterFunc(d.after, fn)
}
