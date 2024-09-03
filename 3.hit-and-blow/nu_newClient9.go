package main

import (
	"fmt"
	"net"
	"slices"
	"sort"
	"strings"
)

func remove(ar []byte, i byte) []byte { //宣言, arはint配列、iはint、arが削除先の配列でiが要素番号
    tmp := make([]byte, len(ar)) //tmoはint配列で、arの長さが配列数
    copy(tmp, ar) //削除されて数字が減ってい行くので、元の配列(ar)をtmpに作り直す。
    return append(tmp[0:i], tmp[i + 1:]...) //tmpのi番目まで + tmpの(i+1)番目を返す、i番目を抜いてる
}

func permutation(ar []byte, digit byte) [][]byte {
    var result [][]byte
	var ar2 []byte
	
	count := 0 

    if len(ar) == int(11 - digit) {
		for i:=1; i<int(12-digit); i++ {
			ar2 = append(ar2, ar[i-1])
			result = append(result,ar2)
			ar2 = []byte{}
			// fmt.Println(int(10-digit)+i)
			// fmt.Println(i)
		}
		// fmt.Println("done")
		 return result
		} 

    for i, a := range ar { 
        for _, b := range permutation(remove(ar, byte(i)), digit) { 
            result = append(result, append([]byte{a}, b...))   
			count += 1
		
        }
		
    }
    return result
}

func ReadWrite(ramNum []byte, conn net.Conn)[]byte{
	buf := make([]byte, 32)
	_, err1 := conn.Write(ramNum)
	if err1 != nil {
		fmt.Println("failed to write")
	}

	_, err2 := conn.Read(buf)
	if err2 != nil {
		fmt.Println("failed to read")
	} else {
		if buf[1] == 0 {
			fmt.Println("correct")
		}
	}
	return buf
}

func firstCheck(permutation [][]byte, conn net.Conn, digit byte) [][]byte {
	firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
	firstNum := []byte{9,8,7,6,5,4,3,2,1}
	
	hit := 0
	blow := 0
	count := 0

	firstSlice = append(firstSlice,firstNum[:digit]...)
	fmt.Println(firstSlice)

	readWrite1 := ReadWrite(firstSlice, conn)
	answerHit := readWrite1[10]
	answerBlow := readWrite1[11]
	selectedArray := [][]byte{}
	numberCount := []byte{}
	
	fmt.Println("最初の送信")
	fmt.Println(readWrite1)

	for _,a := range permutation {
		hit = 0
		blow = 0
		for j,b := range firstNum[:digit] {
			if a[j] == b {
				hit = hit + 1
			} else if slices.Contains(a,b){
				blow = blow + 1
			}
		}
	
		if answerHit == byte(hit) && answerBlow == byte(blow) {
			selectedArray = append(selectedArray, a)
			numberCount = append(numberCount, a...)
			count = count + 1
		} 
	}
	
	fmt.Println(count)
	//fmt.Println(numberCount)
	fmt.Println(strings.Count(string(numberCount), string(1)))


	
	return selectedArray
}

func check(conn net.Conn, selectedArray [][]byte, digit byte) [][]byte {
	var selectedArray2 [][]byte
	//selectedArray3 := make([][]byte,len(selectedArray2))
	//var selectedArray4 [][]byte
	var addedArray []byte
	var narrowedNum []byte
	var digitBox []byte
	numberCount := []byte{}
	var correct int
	
	
	hit := 0
	blow := 0
	count := 0
	//count2 := 0
	
	for h:=0; h<100; h++{  
		
		countResult := []int{
			strings.Count(string(numberCount), string(0)),
			strings.Count(string(numberCount), string(1)),
			strings.Count(string(numberCount), string(2)),
			strings.Count(string(numberCount), string(3)),
			strings.Count(string(numberCount), string(4)),
			strings.Count(string(numberCount), string(5)),
			strings.Count(string(numberCount), string(6)),
			strings.Count(string(numberCount), string(7)),
			strings.Count(string(numberCount), string(8)),
			strings.Count(string(numberCount), string(9))}

		if h%2 == 1 {
			
			original := make([]int, len(countResult))
			//selectedArray3 := make([][]byte,len(selectedArray2))
			selectedArray3 := [][]byte{}
			mostlyCorrect := [][]byte{}
			partlyCorrect := [][]byte{}
			copy(original,countResult)
			
			for x:=1; x<len(slices.Compact(countResult))+1; x++{
				fmt.Println(original)
				slices.Compact(countResult)
				sort.Sort(sort.Reverse(sort.IntSlice(countResult)))
				fmt.Println(slices.Compact(countResult))
				fmt.Println(x, "番目に大きい数字は" , countResult[x-1] )
				
				for f,g := range original{ 
					if g == countResult[x-1] {
						digitBox = append(digitBox, byte(f))
						fmt.Println(digitBox, "送る配列-き数回")
						continue //次に大きな数字に行く
					} 
				}
			}
			narrowedNum = []byte{}
			narrowedNum = append(narrowedNum, digitBox...)
			fmt.Println(narrowedNum)
			digitBox = []byte{}

			//copy(selectedArray3, selectedArray2)
			selectedArray3 = [][]byte{}
		
			for _,t := range selectedArray2{
				correct = 0
				
				for y:=0; y<int(digit); y++{
					// fmt.Println(t)
					// fmt.Println(narrowedNum[y])
					if slices.Contains(t,narrowedNum[y]){
						correct += 1
					}
				}
				kojima2:
				for { 
					if correct == int(digit){
						selectedArray3 = append(selectedArray3, t)
						// fmt.Println("すべてcorrect-終わり！！")
						break kojima2
					} else if correct == int(digit)-1{
						mostlyCorrect = append(mostlyCorrect, t)
						// fmt.Println("ほとんどcorrect")
						break
					} else if correct == int(digit)-2{
						partlyCorrect = append(partlyCorrect, t)
						// fmt.Println("まあまあcorrect")
						break
					} else {
					//fmt.Println("失敗")
						break
					}
				}
			}
			
			fmt.Println(selectedArray2,"もとのやつ")
			fmt.Println(selectedArray3,"完全一致")
			fmt.Println(mostlyCorrect,"ほぼ一致")
			fmt.Println(partlyCorrect,"部分一致")
			selectedArray3 = append(selectedArray3, mostlyCorrect...)
			selectedArray3 = append(selectedArray3, partlyCorrect...)
			fmt.Println(selectedArray3,"合体したやつ")
			
		
			selectedArray3 = [][]byte{}
			selectedArray = [][]byte{}
			firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
			addedArray := append(firstSlice, selectedArray2[0]...)
			readWrite1 := ReadWrite(addedArray, conn)
			count = 0
			numberCount =[]byte{}
		
			
			fmt.Println(h+2 ,"---------回目の送信-奇数---------")
			fmt.Println(addedArray)
			fmt.Println(readWrite1)
			answerHit := readWrite1[10]
			answerBlow := readWrite1[11]
	
			if readWrite1[1] == 0 && h+2 > 12{
				break
			} else if readWrite1[1] == 0{
				break
			}

			for _,a := range selectedArray2{
				hit = 0
				blow = 0
				for j,b := range selectedArray2[0]{
					if a[j] == b {
						hit = hit + 1
					} else  if slices.Contains(a,b){
						blow = blow + 1	
					}
					
				}
				if answerHit == byte(hit) && answerBlow == byte(blow) {
					selectedArray = append(selectedArray, a)
					numberCount = append(numberCount, a...)
					count = count + 1	
				} 
			} 
			
		} else {
			original := make([]int, len(countResult))
			//selectedArray3 := make([][]byte,len(selectedArray2))
			selectedArray3 := [][]byte{}
			mostlyCorrect := [][]byte{}
			partlyCorrect := [][]byte{}
			copy(original,countResult)
			
			for x:=1; x<len(slices.Compact(countResult))+1; x++{
				fmt.Println(original)
				slices.Compact(countResult)
				sort.Sort(sort.Reverse(sort.IntSlice(countResult)))
				fmt.Println(x, "番目に大きい数字は" , countResult[x-1] )
				
				for f,g := range original{ 
					if g == countResult[x-1] {
						digitBox = append(digitBox, byte(f))
						fmt.Println(digitBox, "送る配列-き数回")
					} 
				}
			}
			narrowedNum = []byte{}
			narrowedNum = append(narrowedNum, digitBox...)
			fmt.Println(narrowedNum)
			digitBox = []byte{}

			//copy(selectedArray3, selectedArray2)
			
			for _,t := range selectedArray{
				correct = 0
				
				for y:=0; y<int(digit); y++{
					// fmt.Println(t)
					// fmt.Println(narrowedNum[y])
					if slices.Contains(t,narrowedNum[y]){
						correct += 1
					}
				}
				kojima1:
				for { 
					if correct == int(digit){
						selectedArray3 = append(selectedArray3, t)
						// fmt.Println("すべてcorrect-終わり！！")
						break kojima1
						// fmt.Println(correct)
					} else if correct == int(digit)-1 {
						mostlyCorrect = append(mostlyCorrect, t)
						// fmt.Println("ほとんどcorrect")
						break
					} else if correct == int(digit)-2{
						partlyCorrect = append(partlyCorrect, t)
						// fmt.Println("まあまあcorrect")
						break
					} else {
						//fmt.Println("失敗")
						break
					}
				}
			}
			
			fmt.Println(selectedArray,"もとのやつ")
			fmt.Println(selectedArray3,"完全一致")
			fmt.Println(mostlyCorrect,"ほぼ一致")
			fmt.Println(partlyCorrect,"部分一致")
			selectedArray3 = append(selectedArray3, mostlyCorrect...)
			selectedArray3 = append(selectedArray3, partlyCorrect...)
			fmt.Println(selectedArray3,"合体したやつ")
			
			
			selectedArray2 = [][]byte{}
			firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
			fmt.Println(len(selectedArray3),"これが長さ")
			// if len(selectedArray3) == 0 {
			// 	addedArray = append(firstSlice,narrowedNum...)
			// } else {
			// 	addedArray = append(firstSlice, selectedArray3[0]...)
			// }

			addedArray = append(firstSlice, selectedArray[0]...)
			readWrite1 := ReadWrite(addedArray, conn)
			count = 0
			numberCount =[]byte{}
			narrowedNum = []byte{}	
				
			
			
			fmt.Println(h+2 ,"---------回目の送信-偶数---------")
			fmt.Println(addedArray)
			fmt.Println(readWrite1)
			answerHit := readWrite1[10]
			answerBlow := readWrite1[11]
			
			if readWrite1[1] == 0 && h+2 > 14{
				fmt.Println("13回以上で正解")
				break
			} else if readWrite1[1] == 0{
					break
			}

			for _,a := range selectedArray{
				hit = 0
				blow = 0
				for j,b := range selectedArray[0]{
					if a[j] == b {
						hit = hit + 1
					} else if slices.Contains(a,b){
						blow = blow + 1
						// fmt.Println(a)//二次元配列から順に取り出してる配列
						// fmt.Println(b)//selectedArray[0]の中身
					} 
				}
				if answerHit == byte(hit) && answerBlow == byte(blow){
					selectedArray2 = append(selectedArray2, a)
					numberCount = append(numberCount, a...)
					count = count + 1
				}
			}
			//fmt.Println("偶数回",strings.Count(string(numberCount), string(1)))
			//fmt.Println(countResult)
		}
		
		
	 	fmt.Println(count,"回")	
	}
	
	return selectedArray
}	

func main() {
	var digit byte
	ar := []byte{0,1,2,3,4,5,6,7,8,9}
	
	fmt.Println("桁数を入力してください")
	fmt.Scan(&digit)  
  	digitSlice := []byte{0,digit}

	//for{
	//DIAL
	conn, err := net.Dial("tcp",":9600")
	if err != nil{
		fmt.Println("error: ", err)
	} else {
		fmt.Println("dial success")
	}

	//名前
	data1 := []byte{75,79,74,73}

	//桁数と名前を送る
	data1 = append(digitSlice[:], data1[:]...)
	fmt.Println(data1)
	n1, err1 := conn.Write(data1)
	if (err1 != nil) {
		fmt.Println("cannot write", err1)
		fmt.Println(n1)
	} else {
		fmt.Println("digit sent")
	}

	buf1 := make([]byte, 32)
	n2, err2 := conn.Read(buf1)
	if (err2 != nil){
		fmt.Println("cannot read", err2)
		fmt.Println(buf1[:n2])
	} else {
		fmt.Println("received")
		fmt.Println(buf1)
	}

	
	// //全体の箱を作る

	permutation := permutation(ar,digit)
	//fmt.Println(permutation)
	//初期値を送って、絞る
	
	selectedArray := firstCheck(permutation, conn, digit)

	//絞ったものをさらに送受信しながら何度も絞る

	check(conn, selectedArray,digit)
	
	
//}
	
	
}


  

	