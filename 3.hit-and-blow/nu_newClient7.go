package main

import (
	"fmt"
	"net"
)

func remove(ar []byte, i byte) []byte { //宣言, arはint配列、iはint、arが削除先の配列でiが要素番号
    tmp := make([]byte, len(ar)) //tmoはint配列で、arの長さが配列数
    copy(tmp, ar) //削除されて数字が減ってい行くので、元の配列(ar)をtmpに作り直す。
    return append(tmp[0:i], tmp[i + 1:]...) //tmpのi番目まで + tmpの(i+1)番目を返す
}

func permutation(ar []byte) [][]byte {
    var result [][]byte

    if len(ar) == 1 {return append(result, ar)} 

    for i, a := range ar { 
        for _, b := range permutation(remove(ar, byte(i))) { 
            result = append(result, append([]byte{a}, b...))   
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
		if buf[1] == 0{
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

	firstSlice = append(firstSlice,firstNum[:]...)

	readWrite1 := ReadWrite(firstSlice, conn)
	answerHit := readWrite1[10]
	answerBlow := readWrite1[11]
	selectedArray := [][]byte{}
	
	fmt.Println("最初の送信")
	fmt.Println(readWrite1)

	for _,a := range permutation{
		hit = 0
		blow = 0
		for j,b := range firstNum{
			if a[j] == b {
				hit = hit + 1
			} else {	
				for _,c := range firstNum{
					if a[j] == c {
						blow = blow + 1
					}
				}
			}
		}

		if answerHit == byte(hit) || answerBlow == byte(blow) {
			selectedArray = append(selectedArray, a)
			count = count + 1
		} 
	}
		fmt.Println(count)
		return selectedArray
}

func check(conn net.Conn, selectedArray [][]byte) [][]byte {
	var selectedaArray [][]byte
	hit := 0
	blow := 0
	count := 0

	for h:=0; h<100; h++{  
		
		count = 0 
		
		if h%2 == 1 {
			selectedArray = [][]byte{}
			firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
			addedArray := append(firstSlice, selectedaArray[0]...)
			readWrite1 := ReadWrite(addedArray, conn)
			fmt.Println(h ,"回目の送信-奇数")
			fmt.Println(addedArray)
			fmt.Println(readWrite1)
			answerHit := readWrite1[10]
			answerBlow := readWrite1[11]

			if readWrite1[1] == 0 {
				fmt.Println("正解")
				break
			} 

			for _,a := range selectedaArray{
				hit = 0
				blow = 0
					for j,b := range selectedaArray[0]{
						if a[j] == b {
							hit = hit + 1
						} else {	
							for _,c := range selectedaArray[0]{
								if a[j] == c {
								blow = blow + 1
								}
							}
						}
					}
					if answerHit == byte(hit) || answerBlow == byte(blow) {
						selectedArray = append(selectedArray, a)
						count = count + 1	
					} 
			} 
		} else {
			selectedaArray = [][]byte{}
			firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
			addedArray := append(firstSlice, selectedArray[0]...)
			readWrite1 := ReadWrite(addedArray, conn)
			fmt.Println(h ,"回目の送信-偶数")
			fmt.Println(addedArray)
			fmt.Println(readWrite1)
			answerHit := readWrite1[10]
			answerBlow := readWrite1[11]
			
			if readWrite1[1] == 0 {
				fmt.Println("正解")
				break
			} 

			for _,a := range selectedArray{

				hit = 0
				blow = 0
					for j,b := range selectedArray[0]{
						if a[j] == b {
							hit = hit + 1
						} else {	
							for _,c := range selectedArray[0]{
								if a[j] == c {
								blow = blow + 1
								}
							}
						}
					}

					
				if answerHit == byte(hit)|| answerBlow == byte(blow){
					selectedaArray = append(selectedaArray, a)
					count = count + 1
					
				}	
			} 
		}
	 	fmt.Println(count)	
	}

	return selectedArray
}	

func main() {
	var digit byte
	ar := []byte{0,1,2,3,4,5,6,7,8,9}
	
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
	permutation := permutation(ar)

	//初期値を送って、絞る
	selectedArray := firstCheck(permutation, conn, digit)

	//絞ったものをさらに送受信しながら何度も絞る
	check(conn, selectedArray)
}


  

	