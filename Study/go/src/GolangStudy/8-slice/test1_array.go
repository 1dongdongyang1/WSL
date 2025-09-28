package main

import "fmt"

// 值传递	参数严格
func printArray(myArray [10]int) {
	for _, value := range myArray {
		fmt.Println("value = ", value)
	}
}

func main() {
	var myArray1 [10]int
	
	myArray2 := [10]int{1,2,3,4}

	for i := 0; i < len(myArray1); i++ {
		fmt.Println(myArray1[i])
	}

	for index, value := range myArray2 {
		fmt.Println("index = ", index, "value = ", value)
	}

	printArray(myArray1)
}
