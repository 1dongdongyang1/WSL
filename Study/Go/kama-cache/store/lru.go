package store

import (
	"container/list"
	"time"
)

// ==========================
// 1️⃣ 核心逻辑层：pure LRU
// ==========================
type lruCore struct {
	list      *list.List               // LRU 顺序：最近访问的在前
	items     map[string]*list.Element // key -> 链表节点
	expires   map[string]time.Time     // key -> 过期时间
	maxBytes  int64                    // 最大内存限制
	usedBytes int64                    // 当前已用字节数
	onEvicted func(key string, value Value)
}

// 链表节点存储的内容
type lruEntry struct {
	key   string
	value Value
}

func newLRUCore(maxBytes int64, onEvicted func(string, Value)) *lruCore {
	return &lruCore{
		list:      list.New(),
		items:     make(map[string]*list.Element),
		expires:   make(map[string]time.Time),
		maxBytes:  maxBytes,
		onEvicted: onEvicted,
	}
}

func (c *lruCore) Get(key string) (Value, bool) {
	if ele, ok := c.items[key]; ok {
		entry := ele.Value.(*lruEntry)
		c.list.MoveToFront(ele)
		if expire, ok := c.expires[key]; ok && time.Now().After(expire) {
			c.removeElement(ele)
			return nil, false
		}
		return entry.value, true
	}
	return nil, false
}

func (c *lruCore) Add(key string, value Value, ttl time.Duration) {
	if ele, ok := c.items[key]; ok {
		// 更新
		entry := ele.Value.(*lruEntry)
		c.usedBytes += int64(value.Len() - entry.value.Len())
		entry.value = value
		c.list.MoveToFront(ele)
		c.expires[key] = time.Now().Add(ttl)
	} else {
		// 新增
		entry := &lruEntry{key, value}
		ele := c.list.PushFront(entry)
		c.items[key] = ele
		c.expires[key] = time.Now().Add(ttl)
		c.usedBytes += int64(len(key) + value.Len())
	}

	for c.maxBytes != 0 && c.usedBytes > c.maxBytes {
		c.removeOldest()
	}
}

func (c *lruCore) removeOldest() {
	ele := c.list.Back()
	if ele != nil {
		c.removeElement(ele)
	}
}

func (c *lruCore) removeElement(ele *list.Element) {
	c.list.Remove(ele)
	entry := ele.Value.(*lruEntry)
	delete(c.items, entry.key)
	delete(c.expires, entry.key)
	c.usedBytes -= int64(len(entry.key) + entry.value.Len())
	if c.onEvicted != nil {
		c.onEvicted(entry.key, entry.value)
	}
}

