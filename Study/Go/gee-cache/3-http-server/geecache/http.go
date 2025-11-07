package geecache

import (
	"fmt"
	"log"
	"net/http"
	// "strings"

	"github.com/gin-gonic/gin"
)

const defaultBasePath = "/_geecache"

type HTTPPool struct {
	self 		string
	basePath 	string
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

func (p *HTTPPool) Register(router *gin.Engine) {
	router.GET(p.basePath+"/:groupName/:key", p.handleRequest)
}

func (p *HTTPPool) handleRequest(c *gin.Context) {
	groupName := c.Param("groupName")
	key := c.Param("key")

	p.Log("%s %s", c.Request.Method, c.Request.URL.Path)

	group := GetGroup(groupName)
	if group == nil {
		c.String(http.StatusNotFound, "no such group: "+groupName)
		return
	}

	view, err := group.Get(key)
	if err != nil {
		c.String(http.StatusInternalServerError, err.Error())
		return
	}

	c.Data(http.StatusOK, "application/octet-stream", view.ByteSlice())
}

// func (p *HTTPPool) ServeHTTP(w http.ResponseWriter, r *http.Request) {
// 	if !strings.HasPrefix(r.URL.Path, p.basePath) {
// 		panic("HTTPPool serving unexpected path: " + r.URL.Path)
// 	}
// 	p.Log("%s %s", r.Method, r.URL.Path)
// 	parts := strings.SplitN(r.URL.Path[len(p.basePath):], "/", 2)
// 	if len(parts) != 2 {
// 		http.Error(w, "bad request", http.StatusBadRequest)
// 		return
// 	}

// 	groupName := parts[0]
// 	key := parts[1]

// 	group := GetGroup(groupName)
// 	if group == nil {
// 		http.Error(w, "no such group: " + groupName, http.StatusNotFound)
// 		return
// 	}

// 	view, err := group.Get(key)
// 	if err != nil {
// 		http.Error(w, err.Error(), http.StatusInternalServerError)
// 		return 
// 	}

// 	w.Header().Set("Content-Type", "application/octet-stream")
// 	w.Write(view.ByteSlice())
// }