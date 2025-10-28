package main

import (
	"fmt"
	"reflect"
)

func reflectNum(arg interface{}) {
	fmt.Println("type : ", reflect.TypeOf(arg))
}

/*
	如何理解反射，反射就是通过变量知道变量类型
*/
func main() {
	var num float64 = 1.2345

	reflectNum(num)
}