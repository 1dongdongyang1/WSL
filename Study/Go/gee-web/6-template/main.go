package main

import (
	"fmt"
	"net/http"
	"text/template"
	"time"

	"gee"
)

type student struct {
	Name string
	Age  int8
}

func FormatAsDate(t time.Time) string {
	year, month, day := t.Date()
	return fmt.Sprintf("%d-%02d-%02d", year, month, day)
}


func main() {
	engine := gee.New()
	engine.Use(gee.Logger())
	engine.SetFuncMap(template.FuncMap{
		"FormatAsDate": FormatAsDate,
	})
	engine.LoadHTMLTemplates("templates/*")
	engine.Static("/assets", "./static")

	stu1 := &student{Name: "Deng", Age: 20}
	stu2 := &student{Name: "Li", Age: 22}
	
	engine.GET("/", func(c *gee.Context) {
		c.HTML(http.StatusOK, "css.tmpl", nil)
	})

	engine.GET("/date", func(c *gee.Context) {
		c.HTML(http.StatusOK, "custom_func.tmpl", gee.H{
			"title":  "gee",
			"now":    time.Now(),
		})
	})

	engine.GET("/students", func(c *gee.Context) {
		c.HTML(http.StatusOK, "arr.tmpl", gee.H{
			"title":  "gee",
			"stuArr": [2]*student{stu1, stu2},
		})
	})


	engine.Run("0.0.0.0:9999")
}