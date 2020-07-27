package lrd

import (
	"sync"
	"time"

	"github.com/allegro/bigcache"
)

var o sync.Once

type lrd struct {
	cache       *bigcache.BigCache
	buckets     []sync.RWMutex
	bucketsMask uint64
}

var config bigcache.Config
var Cache *lrd

func InitLRD(life time.Duration) {
	o.Do(func() {
		config.Shards = 1024
		config.LifeWindow = life
		config.CleanWindow = 5 * time.Minute
		config.MaxEntrySize = 512
		config.Verbose = false
		config.HardMaxCacheSize = 8192

		cache, err := bigcache.NewBigCache(config)
		if err != nil {
			panic(err)
		}
		Cache = new(lrd)
		Cache.buckets = make([]sync.RWMutex, 1024)
		Cache.bucketsMask = 1023
		Cache.cache = cache
	})
}

func (c *lrd) Set(key string, value []byte) error {
	mu := &c.buckets[hashSum(key)&c.bucketsMask]
	mu.Lock()
	defer mu.Unlock()
	return c.cache.Set(key, value)
}
