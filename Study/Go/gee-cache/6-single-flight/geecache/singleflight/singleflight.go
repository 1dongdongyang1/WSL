package singleflight

import "sync"

// 如何理解singleflight ?
// 同一时间，多个相同的请求打到服务器上，服务器没有必要处理一样的请求
// singleflight是在客户端处理的，先在客户端判断，只让一个请求过去，其他要发给服务器的请求直接拦截

type call struct {
	wg 		sync.WaitGroup
	val 	interface{}
	err 	error
}

type Group struct {
	mu 	sync.Mutex
	m 	map[string]*call
}

func (g *Group) Do(key string, fn func()(interface{}, error)) (interface{}, error) {
	g.mu.Lock()
	if g.m == nil {
		g.m = make(map[string]*call)
	}
	if c, ok := g.m[key]; ok {
		g.mu.Unlock()
		c.wg.Wait()
		return c.val, c.err
	}
	c := new(call)
	c.wg.Add(1)
	g.m[key] = c
	g.mu.Unlock()

	c.val, c.err = fn()
	c.wg.Done()

	g.mu.Lock()
	delete(g.m, key)
	g.mu.Unlock()
	return c.val, c.err
}