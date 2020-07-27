package v1

import (
	"container/heap"
	"sync"
)

type DownloadManager struct {
	mu      sync.RWMutex
	tasks   heap.Interface
	existed map[string]bool
}
