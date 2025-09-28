package main

import "fmt"

// 声明一种行的数据类型 myint，是int的一个别名
type myint int

// 定义一个结构体
type Book struct {
	title string
	auth string
}

func changeBook(book Book)  {
	// 值传递	
	book.auth = "111"
}

func changeBook2(book *Book) {
	// 指针传递
	book.auth = "222"
}

func main() {
	var a myint = 10
	fmt.Println(a)

	var book1 Book
	book1.title = "Golang"
	book1.auth = "ddy"

	fmt.Println(book1)
}
