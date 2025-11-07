package geecache

import (
	"fmt"
	"geecache/consistenthash"
	"io"
	"log"
	"net/http"
	"net/url"
	"sync"
	"strings"

	// "github.com/gin-gonic/gin"
)

const (
	defaultBasePath = "/_geecache"
	defaultReplicas = 50
)

type HTTPPool struct {
	self 		string	// ip + port
	basePath 	string	// path
	mu 			sync.Mutex
	peers		*consistenthash.Map
	httpGetters map[string]*httpGetter
}

func NewHTTPPool(self string) *HTTPPool {
	return &HTTPPool{
		self:  		self,
		basePath:   defaultBasePath,
	}
}

func (p *HTTPPool) Log(format string, v ...interface{}) {
	log.Printf("[Server %s] %s", p.self, fmt.Sprintf(format, v...))
}

// func (p *HTTPPool) Register(router *gin.Engine) {
// 	router.GET(p.basePath+"/:groupName/:key", p.handleRequest)
// }

// func (p *HTTPPool) handleRequest(c *gin.Context) {
// 	groupName := c.Param("groupName")
// 	key := c.Param("key")

// 	p.Log("%s %s", c.Request.Method, c.Request.URL.Path)

// 	group := GetGroup(groupName)
// 	if group == nil {
// 		c.String(http.StatusNotFound, "no such group: "+groupName)
// 		return
// 	}

// 	view, err := group.Get(key)
// 	if err != nil {
// 		c.String(http.StatusInternalServerError, err.Error())
// 		return
// 	}

// 	c.Data(http.StatusOK, "application/octet-stream", view.ByteSlice())
// }

func (p *HTTPPool) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	if !strings.HasPrefix(r.URL.Path, p.basePath) {
		panic("HTTPPool serving unexpected path: " + r.URL.Path)
	}
	p.Log("%s %s", r.Method, r.URL.Path)
	parts := strings.SplitN(r.URL.Path[len(p.basePath):], "/", 2)
	if len(parts) != 2 {
		http.Error(w, "bad request", http.StatusBadRequest)
		return
	}

	groupName := parts[0]
	key := parts[1]

	group := GetGroup(groupName)
	if group == nil {
		http.Error(w, "no such group: " + groupName, http.StatusNotFound)
		return
	}

	view, err := group.Get(key)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return 
	}

	w.Header().Set("Content-Type", "application/octet-stream")
	w.Write(view.ByteSlice())
}

func (p *HTTPPool) Set(peers ...string) {
	p.mu.Lock()
	defer p.mu.Unlock()

	p.peers = consistenthash.New(defaultReplicas, nil)
	p.peers.Add(peers...)
	p.httpGetters = make(map[string]*httpGetter, len(peers))
	for _, peer := range peers {
		p.httpGetters[peer] = &httpGetter{
			baseURL: peer + p.basePath,
		}
	}
}

func (p *HTTPPool) PickPeer(key string) (PeerGetter, bool) {
	p.mu.Lock()
	defer p.mu.Unlock()

	if peer := p.peers.Get(key); peer != "" && peer != p.self {
		p.Log("Pick peer %s", peer)
		return p.httpGetters[peer], true
	}
	return nil, false
}

var _ PeerPicker = (*HTTPPool)(nil)

type httpGetter struct {
	baseURL string
}

func (h *httpGetter) Get(group string, key string) ([]byte, error) {
	u := fmt.Sprintf(
		"%v%v/%v",
		h.baseURL,
		url.QueryEscape(group),
		url.QueryEscape(key),
	)
	res, err := http.Get(u)
	if err != nil {
		return nil, err
	}
	defer res.Body.Close()

	if res.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("server returned: %v", res.Status)
	}

	bytes, err := io.ReadAll(res.Body)
	if err != nil {
		return nil, fmt.Errorf("reading response body: %v", err)
	}

	return bytes, nil
}

var _ PeerGetter = (*httpGetter)(nil)