package main

import "fmt"

// 如果类名首字母大写，表示其他包也能访问
type Hero struct {
	// 类属性首字母大写，表示public，小写表示private
	Name	string
	Ad 	 	int 
	Level	int
}

func (this Hero) Show() {
	fmt.Println("Name = ", this.Name)
}

func (this Hero) GetName() string {
	return this.Name
}

func (this *Hero) SetName(newName string) {
	// this 是调用该方法对象的一个拷贝
	// *this 是指针
	this.Name = newName
}

func main() {
	hero := Hero{Name: "ddy", Ad: 10, Level: 2}

	hero.Show()
}