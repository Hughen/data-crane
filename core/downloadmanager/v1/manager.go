package v1

import (
	"container/heap"
	"sync"
)

type S3AuthContext struct {
	AccessKey string
	SecretKey string
}

type S3Object struct {
	URI     string
	AuthCtx S3AuthContext
	mu      sync.RWMutex
}

type DownloadManager struct {
	mu              sync.RWMutex
	tasks           heap.Interface
	workers         []*S3Object
	currentTasksMap map[string]bool
}
