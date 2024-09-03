package main

import (
	"fmt"
	"net"
	// "strconv"
)

func remove(ar []int, i int) []int { //宣言, arはint配列、iはint、arが削除先の配列でiが要素番号
    tmp := make([]int, len(ar)) //tmoはint配列で、arの長さが配列数
    copy(tmp, ar) //削除されて数字が減ってい行くので、元の配列(ar)をtmpに作り直す。
    return append(tmp[0:i], tmp[i + 1:]...) //tmpのi番目まで + tmpの(i+1)番目を返す
}

func permutation(ar []int) [][]int {
    var result [][]int
    if len(ar) == 1 {return append(result, ar)} 
    for i, a := range ar { 
        for _, b := range permutation(remove(ar, i)) { 
            result = append(result, append([]int{a}, b...))   
        }
    }
    return result
}

func ReadWrite(answerSlice2 []byte, conn net.Conn)[]byte{
	buf := make([]byte, 32)
	_, err1 := conn.Write(answerSlice2)
	if err1 != nil {
		fmt.Println("failed to write")
	}
	_, err2 := conn.Read(buf)
	if err2 != nil {
		fmt.Println("failed to read")
	} else {
		fmt.Println(answerSlice2)
			if buf[1] == 0{
				fmt.Println("correct")
			}
	}
	return buf
}

func firstCheck(answerSlice2 []byte, conn net.Conn, all [][]byte)[][]byte {
firstNum := []byte{1,2,3,4,5,6,7,8,9}
var selectedArray [][]byte
	for i:=0; i<10; i++{
		answerSlice2 = append(answerSlice2[:10],firstNum[:]...)

		a := ReadWrite(answerSlice2, conn)
		count := 0

		fmt.Println(a)


		for _,a := range all {
			for j,b := range answerSlice2{
				if a[j] == b {
					count = count + 1
				} else if count > 1 {
					selectedArray = append(selectedArray, answerSlice2)
					// fmt.Println(selectedArray)	
				}
			}
		}
	}
	fmt.Println(all)

	return selectedArray
} 

	

func addAnswer(answerSlice []byte, digit byte, conn net.Conn) []byte {

	data2 := []byte{0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	answerSlice2 := data2[0:10]

	firstCheck(answerSlice2, conn, all)
	fmt.Println(all)
	
	

	kojima:
	for i := 0; i < 10; i++ {		
		for j := 0; j < 10; j++ {
			if j == i {
				continue
			}
			for k := 0; k < 10; k++ {
				if k == i || k == j {
					continue
				} else if  digit == 3  {
					answerSlice2 = append(answerSlice2[:10],byte(i))
					answerSlice2 = append(answerSlice2[:],byte(j))
					answerSlice2 = append(answerSlice2[:],byte(k))

					
					ReadWrite := ReadWrite(answerSlice2, conn)
					fmt.Println(ReadWrite)
					if ReadWrite[1] == byte(0) {
						break kojima
					} 
				}
			}
		}
	}
		return answerSlice2	
}	

func main() {
	var digit byte
	
	
	
	fmt.Println("桁数を入力してください")
	fmt.Scan(&digit)  
  	digitSlice := []byte{0,digit}

	

	//DIAL
	conn, err := net.Dial("tcp",":9600")
	if err != nil{
		fmt.Println("error: ", err)
	} else {
		fmt.Println("dial success")
	}

	data1 := []byte{75,79,74,73}

	data1 = append(digitSlice[:], data1[:]...)
	fmt.Println(data1)

	all := permutation()
	permutation([]int{1,2,3,4,5,6,7,8,9})
	
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

	addAnswer(digitSlice,digit,conn)
}


  

	