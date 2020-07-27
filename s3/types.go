package s3

import (
	"math/rand"
	"sync"
	"time"
)

type Status struct {
	StartedTime  time.Time
	FinishedTime time.Time
	Size         uint32
	HTTPCode     int
	ETag         string
	Location     string
}

type Object struct {
	URI    string
	status *Status
	mu     sync.RWMutex
}

func NewObject(uri string) *Object {
	return &Object{URI: uri}
}

func (o *Object) SubStatus() *Status {
	if o.status != nil {
		return o.status
	}

	n := rand.Intn(30)
	if n < 5 {
		n = 5
	}
	interval := time.Duration(n) * time.Millisecond
	for {
		o.mu.RLock()
		if o.status == nil {
			o.mu.RUnlock()
			time.Sleep(interval)
		}
		o.mu.RUnlock()
		break
	}

	return o.status
}

func (o *Object) SetStatus(status *Status) {
	o.status = status
}

func (o *Object) HoldOn() *Object {
	o.mu.Lock()
	return o
}

func (o *Object) Publish() {
	o.mu.Unlock()
}
