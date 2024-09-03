package main

import "fmt"
import "strconv"
import "strings"
//import "slices"
import "reflect"
import "math/rand"
import	"slices"


func ramdomNum(digit int) string {
	targetList := []int{0,1,2,3,4,5,6,7,8,9}
	slice := make([]int,digit)
	unique := make([]int,digit)

	for i:=0; i<digit; i++{
		index := rand.Intn(len(targetList))
		result := targetList[index]
		slice[i] = result
	}

	for n:=0; n<100; n++{
		index := rand.Intn(len(targetList))
		result := targetList[index]
		unique =append(unique,result) 
		slices.Sort(slice)
		unique = slices.Compact(slice)
		
		if(len(unique) == digit){
			rand.Shuffle(len(unique),func(i, j int){
				unique[i],unique[j] = unique[j], unique[i]
			})
			if(unique[0] == 0){
				unique[0] = 1
			}	
			break
		}
	}
	strUnique := []string{}
	for _, v := range unique {
		strUnique = append(strUnique, strconv.Itoa(v))
	}
	return strings.Join(strUnique,"")
}

func main() {

	var digit int
	var answer1 string
	//var answer2 string
	var result string
	//var x bool =true
	//var count int 

	//初動
	fmt.Println("桁数を入力してください")
	fmt.Scan(&digit)
	ramdomNum := ramdomNum(digit)
	answer1 = ramdomNum
	
	for i:=0; i < 100; i++{
		hitCount := 0
		blowCount := 0
		var count int = i+1
		fmt.Println(count,"回目の回答を入力してください")
		fmt.Scan(&result)
		slicedA := strings.Split(answer1, "")
		slicedR := strings.Split(result, "")

		for n:=0; n<len(result); n++ {
			//fmt.Println(x == (slicedR[n] == slicedA[n]))
		 
			if(digit == len(result)){
				if(reflect.DeepEqual(slicedA,slicedR) == true){ //完全一致
					fmt.Println("正解")
					return;
				} else if(slicedR[n] == slicedA[n]== true){ //hit(桁のみの完全一致)
					hitCount++
				} else if(strings.Contains(answer1,slicedR[n])){
					blowCount++
				}
			} else {	
			fmt.Println("指定した桁数で回答してください")
			}
		}
		fmt.Println(hitCount, "hit", blowCount, "blow")
	}
	return;
}	






