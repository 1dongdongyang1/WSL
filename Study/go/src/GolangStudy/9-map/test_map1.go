package main

import "fmt"

func main() {
	// 声明1 key是string，value是string
	var myMap1 map[string]string
	if myMap1 == nil {
		fmt.Println("myMap1 是一个空map")
	}

	myMap1 = make(map[string]string, 10)
	myMap1["one"] = "java"
	myMap1["two"] = "C++"
	myMap1["three"] = "python"
	fmt.Println(myMap1)

	// 声明2
	myMap2 := make(map[int]string)
	myMap2[1] = "java"

	// 声明3
	myMap3 := map[string]string {
		"one":	"php",
	}
	fmt.Println(myMap3)
}
