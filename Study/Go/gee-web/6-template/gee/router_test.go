package gee

import (
	"fmt"
	"reflect"
	"testing"
)

func newTestRouter() *router {
	r := newRouter()
	r.addRoute("GET", "/", nil)
	r.addRoute("GET", "/hello/:name", nil)
	r.addRoute("GET", "/hello/b/c", nil)
	r.addRoute("GET", "/hi/:name", nil)
	r.addRoute("GET", "/assets/*filepath", nil)
	return r
}

func TestParsePattern(t *testing.T) {
	ok := reflect.DeepEqual(parsePattern("/p/:lang/doc"), []string{"p", ":lang", "doc"})
	ok = ok && reflect.DeepEqual(parsePattern("/p/:lang/"), []string{"p", ":lang"})
	ok = ok && reflect.DeepEqual(parsePattern("/p/:lang/*"), []string{"p", ":lang", "*"})
	ok = ok && reflect.DeepEqual(parsePattern("/p/*filepath"), []string{"p", "*filepath"})
	if !ok {
		t.Fatal("parsePattern failed")
	}
}

func TestGetRoute(t *testing.T) {
	r := newTestRouter()
	n, params := r.getRoute("GET", "/hello/geektutu")
	if n == nil {
		t.Fatal("nil shouldn't be returned")
	}
	if n.pattern != "/hello/:name" {
		t.Fatal("should match /hello/:name")
	}
	if params["name"] != "geektutu" {
		t.Fatal("name should be equal to 'geektutu'")
	}

	fmt.Printf("matched route: %s, params['name']: %s\n", n.pattern, params["name"])
}