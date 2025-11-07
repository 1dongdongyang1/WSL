package lru

import "container/list"

type Value interface{
	Len() int 
}

type entry struct {
	key 	string
	value 	Value
}

type Cache struct {
	maxBytes int64
	nbytes 	 int64
	ll 		 *list.List
	cache 	 map[string]*list.Element
	OnEvicted func(key string, value Value)
}

func New(maxBytes int64, onEvicted func(string, Value)) *Cache {
	return &Cache{
		maxBytes:  maxBytes,
		ll:		   list.New(),
		cache:     make(map[string]*list.Element),
		OnEvicted: onEvicted,
	}
}

// 查
func (c *Cache) Get(key string) (value Value, ok bool) {
	// 哈希 O(1) 更新节点在链表的位置
	if ele, ok := c.cache[key]; ok {
		c.ll.MoveToFront(ele)
		kv := ele.Value.(*entry)
		return kv.value, true
	}
	return 
}

// 删
func (c *Cache) RemoveOldest() {
	// 哈希 + 链表 + 内存 + (执行回调)
	ele := c.ll.Back()
	if ele != nil {
		c.ll.Remove(ele)
		kv := ele.Value.(*entry)	
		delete(c.cache, kv.key)
		c.nbytes -= int64(len(kv.key)) + int64(kv.value.Len())
		if c.OnEvicted != nil {
			c.OnEvicted(kv.key, kv.value)
		}
	}
}

// 增 + 改
func (c *Cache) Add(key string, value Value) {
	if ele, ok := c.cache[key]; ok {
		// 1. 节点存在，更新节点，节点在链表中的顺序 + key对应的value值 + 内存大小
		c.ll.MoveToFront(ele)
		kv := ele.Value.(*entry)
		c.nbytes += int64(value.Len()) - int64(kv.value.Len())
		kv.value = value
	} else {
		// 2. 节点不存在，加入节点，哈希表 + 链表 + 内存大小
		ele := c.ll.PushFront(&entry{key, value})
		c.cache[key] = ele
		c.nbytes += int64(len(key)) + int64(value.Len())
	}
	// 超过设置的最大内存后，删掉最久没有使用的节点
	for c.maxBytes != 0 && c.maxBytes < c.nbytes {
		c.RemoveOldest()
	}
}

func (c *Cache) Len() int {
	return c.ll.Len()
}