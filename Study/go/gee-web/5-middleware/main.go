package main

import (
	"log"
	"net/http"
	"time"

	"gee"
)

func onlyForV1() gee.HandlerFunc {
	return func(c *gee.Context) {
		// Start timer
		t := time.Now()
		c.Fail(500, "Internal Server Error")
		// Calculate resolution time
		log.Printf("[%d] %s in %v", c.StatusCode, c.Req.RequestURI, time.Since(t))
	}
}

func main() {
	engine := gee.New()
	engine.Use(gee.Logger())
	engine.GET("/", func(c *gee.Context) {
		c.String(http.StatusOK, "Hello Middleware!\n")
	})

	v1 := engine.Group("/v1")
	v1.Use(onlyForV1())
	{
		v1.GET("/hello", func(c *gee.Context) {
			// expect /v1/hello?name=gee
			c.String(http.StatusOK, "hello %s, you're at %s\n", c.Query("name"), c.Path)
		})
	}


	engine.Run(":9999")
}