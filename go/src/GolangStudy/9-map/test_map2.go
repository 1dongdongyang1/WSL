package main

import "fmt"

// 传引用
func printMap(myMap map[string]string) {
	for k, v := range myMap {
		fmt.Println("k = ", k)
		fmt.Println("v = ", v)
	}
}

func main() {
	cityMap := make(map[string]string)

	// 增
	cityMap["ddy"] = "ym"
	cityMap["ym"] = "ddy"

	// 删
	delete(cityMap, "ddy")

	printMap(cityMap)
}