package main

import "fmt"

// interface{}是万能数据类型
func myFunc(arg interface{}) {
	fmt.Println(arg)

	// interface{} 该如何区分底层数据类型
	// interface{} 提供"类型断言"的机制
	value, ok := arg.(string)
	if !ok {
		// fmt.Println("arg is not string type")
		fmt.Printf("arg is %T type\n", value)
	} else {
		fmt.Println("arg is string type, value = ", value)
	}
}

type Book struct {
	auth string
}

func main() {
	book := Book{"Golang"}

	myFunc(book)
	myFunc(100)
		
	myFunc('a')

}