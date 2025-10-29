package main

import (
	"net/http"

	"gee"
)

func main() {
	engine := gee.New()
	engine.GET("/index", func(c *gee.Context) {
		c.HTML(http.StatusOK, "<h1>Hello Gee</h1>")
	})

	v1 := engine.Group("/v1")
	{
		v1.GET("/hello", func(c *gee.Context) {
			c.String(http.StatusOK, "hello v1")
		})
		
		v1.GET("/", func(c *gee.Context) {
			c.String(http.StatusOK, "URL.Path = %q\n", c.Req.URL.Path)
		})
	}
	v2 := engine.Group("/v2")
	{
		v2.GET("/hello", func(c *gee.Context) {
			c.String(http.StatusOK, "hello v2")
		})
		v2.POST("/login", func(c *gee.Context) {
			c.JSON(http.StatusOK, gee.H{
				"username": c.PostForm("username"),
				"password": c.PostForm("password"),
			})
		})
	}

	engine.Run(":9999")
}