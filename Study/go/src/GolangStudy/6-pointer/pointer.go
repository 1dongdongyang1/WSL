package main

import "fmt"

func swap(pa *int, pb *int) {
	tmp := *pa
	*pa = *pb
	*pb = tmp
}

func main() {
	a := 10
	b := 20
	swap(&a, &b)
	fmt.Println(a,b)
}
