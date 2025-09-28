package main

import "fmt"

// 引用传递
func printArray(myArray []int) {
	for _, value := range myArray {
		fmt.Println("value = ", value)
	}
}

func main() {
	myArray := []int{1,2,3,4}	// 动态数组，切片slice
	fmt.Printf("myArray type is %T\n", myArray)
	printArray(myArray)
}