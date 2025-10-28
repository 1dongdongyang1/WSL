package main

import "fmt"

type Human struct {
	name string
	sex string
}

func (this *Human) Eat() {
	fmt.Println("Human.Eat()...")
}

type SuperMan struct {
	Human 	// SuperMan类继承了Human类的方法

	level int
}

// 重定义父类的方法Eat()
func (this *SuperMan) Eat() {
	fmt.Println("SuperMan.Eat()...")
}

func main() {
	h := Human{"ddy", "shape"}

	h.Eat()

	// s := SuperMan{Human{"ym", "woman"}, 1}
	var s SuperMan
	s.name = "ym"
	s.sex = "woman"
	s.level = 1

	s.Eat()
}