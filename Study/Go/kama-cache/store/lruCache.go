package store

import (
	"container/list"
	"sync"
	"time"
)

// ==========================
// 2️⃣ 封装层：并发安全 + 生命周期
// ==========================
type lruCache struct {
	mu              sync.RWMutex
	core            *lruCore
	cleanupInterval time.Duration
	cleanupTicker   *time.Ticker
	closeCh         chan struct{}
}

func NewLRUCache(opts Options) *lruCache {
	cleanupInterval := opts.CleanupInterval
	if cleanupInterval <= 0 {
		cleanupInterval = time.Minute
	}

	cache := &lruCache{
		core:            newLRUCore(opts.MaxBytes, opts.OnEvicted),
		cleanupInterval: cleanupInterval,
		cleanupTicker:   time.NewTicker(cleanupInterval),
		closeCh:         make(chan struct{}),
	}
	go cache.cleanupLoop()
	return cache
}

func (c *lruCache) Get(key string) (Value, bool) {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.core.Get(key)
}

func (c *lruCache) Set(key string, value Value) error {
	return c.SetWithExpiration(key, value, 0)
}

func (c *lruCache) SetWithExpiration(key string, value Value, ttl time.Duration) error {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.core.Add(key, value, ttl)
	return nil
}

func (c *lruCache) Delete(key string) bool {
	c.mu.Lock()
	defer c.mu.Unlock()

	if elem, ok := c.core.items[key]; ok {
		c.core.removeElement(elem)
		return true
	}
	return false
}

func (c *lruCache) Clear() {
	c.mu.Lock()
	defer c.mu.Unlock()

	// 如果设置了回调函数，遍历所有项调用回调
	if c.core.onEvicted != nil {
		for _, elem := range c.core.items {
			entry := elem.Value.(*lruEntry)
			c.core.onEvicted(entry.key, entry.value)
		}
	}

	c.core.list.Init()
	c.core.items = make(map[string]*list.Element)
	c.core.expires = make(map[string]time.Time)
	c.core.usedBytes = 0
}

func (c *lruCache) Close() {
	close(c.closeCh)
	c.cleanupTicker.Stop()
}

func (c *lruCache) Len() int {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return c.core.list.Len()
}

func (c *lruCache) cleanupLoop() {
	for {
		select {
		case <-c.cleanupTicker.C:
			c.mu.Lock()
			for k, expire := range c.core.expires {
				if time.Now().After(expire) {
					if ele, ok := c.core.items[k]; ok {
						c.core.removeElement(ele)
					}
				}
			}
			c.mu.Unlock()
		case <-c.closeCh:
			return
		}
	}
}