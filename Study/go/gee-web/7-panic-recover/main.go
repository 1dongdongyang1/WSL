package main

import (
	"net/http"

	"gee"
)

func main() {
	engine := gee.Default()
	engine.GET("/", func(c *gee.Context) {
		c.String(http.StatusOK, "Hello Gee")
	})
	engine.GET("/panic", func(c *gee.Context) {
		names := []string{"ddy"}
		// 这里会发生数组越界的恐慌错误
		c.String(http.StatusOK, names[100])
	})

	engine.Run("0.0.0.0:9999")
}