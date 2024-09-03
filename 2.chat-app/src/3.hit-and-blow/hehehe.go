package main

import "fmt"

func main() {
/*
var digit int
fmt.Println("桁数を入力してください")
fmt.Scan(&digit)
*/

type Condition struct{
	Digit int
} 

var answer1 int
	//var answer2 int
	var result int

	for i:=0; i < 100; i++ {
		fmt.Println("%s回目の回答を入力してください", i+1)
		fmt.Scan(&result)
			if (answer1 == result){
				fmt.Println("あってます")
			}
	}
}


/*
type Condition struct{
	Digit int
} //構造体


func NewCondition(digit int) Condition {
	return Condition{
		Digit: digit, 
	} //コンストラクタ(属性)
}
*/

/*

func (c Condition) compareNumber(result []string){
	var answer1 int
	//var answer2 int
	var result int

	for i:=0; i < 100; i++ {
		fmt.Println("%s回目の回答を入力してください", i+1)
		fmt.Scan(&result)
			if (answer1 == result){
				fmt.Println("あってます")
			}
	}
}
*/