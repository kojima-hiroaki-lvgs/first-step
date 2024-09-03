package main

import (
	"fmt"
	"time"
)

//スライスの 位置 i の要素を除いたスライスを返す（arを破壊しないようコピーしている）
func remove(ar []int, i int) []int { //i=0
    tmp1 := make([]int, len(ar)) //コピー先のスライスを作成
    copy(tmp1, ar) 
    return append(tmp1[0:i], tmp1[i+1:]...) //arのコピーのi番目が取り除かれる処理
}

func permutation(ar []int, digit int) [][]int {
    var result [][]int
    var tmp2 []int
    var none [][]int

    if len(ar) == 11 - digit {
        for b:=1; b<=int(11 - digit); b++ { 
			tmp2 = append(tmp2, ar[b-1]) 
			result = append(result,tmp2)
			tmp2 = []int{}
		}
	} else if len(ar) < 11 - digit { 
        return none
    } 
    for i, a := range ar {
        for _, b := range permutation(remove(ar, i),digit) { //11以下だとappendが足りない=全通りでない
            result = append(result, append([]int{a}, b...)) 
        }
    }
    return result
}

func main() {
    s := time.Now()
    var digit int
    fmt.Println("桁数を送信してください")
    fmt.Scan(&digit)
    permutation([]int{0,1,2,3,4,5,6,7,8,9}, digit)
    fmt.Println("done")
    fmt.Printf("process time: %s\n", time.Since(s))
}