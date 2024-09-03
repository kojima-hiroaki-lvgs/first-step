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
    return append(tmp[0:i], tmp[i+1:]...) //tmpのi番目まで + tmpの(i+1)番目を返す、i番目を抜いてる
}

func permutation(ar []byte, digit byte) [][]byte {
    var result [][]byte
	var tmp []byte
	var none [][]byte

    if len(ar) == int(11 - digit) { //⚫︎指定した桁数の順列を作るための処理,11-digitの数の中から終わりの桁を決めていく
		for i:=1; i<=int(11-digit); i++ { //残りの桁を
			tmp = append(tmp, ar[i-1]) //i=0だとremoveのappendで-1になってしまうためi=1にして,ここではar[0]が欲しいため-1している
			result = append(result,tmp)
			tmp = []byte{}
		}
	} else if len(ar) < int(11 - digit) {
        return none
    }
	
    for i, a := range ar { 
        for _, b := range permutation(remove(ar, byte(i)), digit) { //permutationの再帰に入る
            result = append(result, append([]byte{a}, b...)) //再帰の処理が終了すると入る、上のif文のresultに1つ上の層のaをappend
        }
    }
    return result
}

func WriteRead(ramNum []byte, conn net.Conn)[]byte{ //送信と受信の処理で、返り値は受信データ
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

func firstCheck(permutation [][]byte, conn net.Conn, digit byte) ([][]byte,[]int) { //自分で決めたfirstNumを送る
	firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
	firstNum := []byte{9,8,7,6,5,4,3,2,1}
	hit := 0
	blow := 0
	count := 0

	firstSlice = append(firstSlice,firstNum[:digit]...) //[:]で桁数分append
	WriteRead1 := WriteRead(firstSlice, conn)
	answerHit := WriteRead1[10]
	answerBlow := WriteRead1[11]
	selectedArray := [][]byte{}
	numberCount := []byte{}
	
	fmt.Println("最初の送信")
	fmt.Println(WriteRead1)

	for _,a := range permutation { //permutationが全体の約363万通り
		hit = 0
		blow = 0
		for j,b := range firstNum[:digit] { //permutationのうちの1つの配列が、配列firstNumの1つの数字と同じ位置かつ等しいかを判別
			if a[j] == b {
				hit = hit + 1
			} else if slices.Contains(a,b){ //同じ位置かは考慮せずに、配列firstNumの1つの数字が指定の入れるに含まれているかを確認
				blow = blow + 1
			}
		}
		if answerHit == byte(hit) && answerBlow == byte(blow) { //サーバーから帰ってきたhit&blowと全体の配列と比較して得たhit&blowを比較して、同じものを絞る
			selectedArray = append(selectedArray, a)
			numberCount = append(numberCount, a...) //頻出の数字を後ほどカウントするために1次元配列に絞った配列を入れる
			count = count + 1
		} 
	}

	firstCountResult := []int{ //絞った配列の数値が全て入っている1次元配列に0~9の数字がいくつ含まれているかをカウントして、1次元配列に入れる
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

	fmt.Println(firstCountResult)

	return selectedArray, firstCountResult
}

func check(conn net.Conn, selectedArray [][]byte, digit byte, firstCountResult[]int) [][]byte {
	var selectedArray2 [][]byte
	var addedArray []byte
	var digitBox []byte
	var correct int
	numberCount := []byte{}
	hit := 0
	blow := 0
	count := 0

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

		if h%2 == 0  { //全体から絞り込んだ配列を入れる箱を同一にすると、二次元配列を消去するタイミングがないため、奇数偶数に分けて箱も2つ作って交互に出し入れ消去している
			original := make([]int, len(countResult))
			selectedArray3 := [][]byte{}
			mostlyCorrect := [][]byte{}
			partlyCorrect := [][]byte{}
			others := [][]byte{}
			digitCount := []byte{}
			selectedDigit := []byte{}
			tmp0 := [][]byte{}
			tmp1 := [][]byte{}
			tmp2 := [][]byte{}
			tmp3 := [][]byte{}
 			tmp4 := [][]byte{}
			tmp5 := [][]byte{}
			tmp6 := [][]byte{}
			tmp7 := [][]byte{}
			tmp8 := [][]byte{}

			if h == 0 { //⚫︎1回目にfirstcheckでカウントした頻出数を初回のみ代入する
				countResult = firstCountResult
			}
			sort.Sort(sort.Reverse(sort.IntSlice(countResult)))

			if h >= 6 { //7回目以上の場合(firstCheckが一回目)、1回目からだと母数が多くて処理に時間がかかる
				correctCount := 0
				for i:=1; i<10; i++{
					for _,o := range selectedArray{
						digitCount = append(digitCount, o[i-1])//全配列の1~9ｹﾀを各箱に入れる
						digitCount2 := []int{ //入れた箱に0~9の数字が何個含まれているのかを数えて入れる。
							strings.Count(string(digitCount), string(0)),
							strings.Count(string(digitCount), string(1)),
							strings.Count(string(digitCount), string(2)),
							strings.Count(string(digitCount), string(3)),
							strings.Count(string(digitCount), string(4)),
							strings.Count(string(digitCount), string(5)),
							strings.Count(string(digitCount), string(6)),
							strings.Count(string(digitCount), string(7)),
							strings.Count(string(digitCount), string(8)),
							strings.Count(string(digitCount), string(9))}
						tmp := make([]int, len(digitCount2)) 
						copy(tmp,digitCount2) //digitCount2の値を操作するためコピーしておく
						sort.Sort(sort.Reverse(sort.IntSlice(digitCount2))) //降順
						shortened := slices.Compact(digitCount2) //重複を消す

						for d,e:= range tmp { //特定のｹﾀにある数字の出現数を最大値と比較して、同一ならappendする
							if e == shortened[0]{
								selectedDigit = append(selectedDigit, byte(d))//index番号と数字が対応している
								break
							}
						}
						digitCount2 = []int{}
					}	
					digitCount = []byte{}
				}
				for f,g := range selectedArray{
					for h,i := range selectedArray[f]{
							if i == selectedDigit[h]{ //箱の中の1つの配列の1ｹﾀ目にある要素が、全体の1ｹﾀの最頻の数字と同じ時にカウント
								correctCount++ 
							}
						}
						if correctCount == int(digit){ //カウントごとに箱に入れる
							tmp0 = append(tmp0, g)
							correctCount = 0
						} else if correctCount == int(digit)-1{
							tmp1 = append(tmp1, g) 
							correctCount = 0
						} else if correctCount == int(digit)-2{
							tmp2 = append(tmp2, g)
							correctCount = 0
						} else if correctCount == int(digit)-3{
							tmp3 = append(tmp3, g)
							correctCount = 0
						} else if correctCount == int(digit)-4{
							tmp4 = append(tmp4, g)
							correctCount = 0
						} else if correctCount == int(digit)-5{
							tmp5 = append(tmp5, g)
							correctCount = 0
						} else if correctCount == int(digit)-6{
							tmp6 = append(tmp6, g)
							correctCount = 0
						} else if correctCount == int(digit)-7{
							tmp7 = append(tmp7, g)
							correctCount = 0
						} else if correctCount == int(digit)-8{
							tmp8 = append(tmp8, g)
							correctCount = 0
						}
					selectedArray3 = append(selectedArray3,tmp0...) //selectedDigitに近いものからappendしていく
					selectedArray3 = append(selectedArray3,tmp1...) //最終的にselectedDigit3[0]を選ぶため、出現数の多いものが選ばれる
					selectedArray3 = append(selectedArray3,tmp2...)
					selectedArray3 = append(selectedArray3,tmp3...)
					selectedArray3 = append(selectedArray3,tmp4...)
					selectedArray3 = append(selectedArray3,tmp5...)
					selectedArray3 = append(selectedArray3,tmp6...)
					selectedArray3 = append(selectedArray3,tmp7...)
					selectedArray3 = append(selectedArray3,tmp8...)	
				}				
			} else {	

			copy(original,countResult) //⚫︎頻出数の配列のインデックスが実際の数字と対応している&&countResultを操作するため、原型を取っておく。
			
			for x:=0; x<len(slices.Compact(countResult)); x++{ ///⚫︎頻出数をカウントする配列→出現数の多い数字を降順にした配列
				slices.Compact(countResult) //出現数の重複をなくす
				sort.Sort(sort.Reverse(sort.IntSlice(countResult))) //出現数を降順にする			
				for f,g := range original{ 
					if g == countResult[x] { //操作前の配列の要素を1つずつ、出現数の多いものと比較して合致しているか調べる。
						digitBox = append(digitBox, byte(f)) //インデックスと実際の数字が紐づいているため、合致していればインデックス番号をappendする。
					} 
				}
			}

			for _,t := range selectedArray{ //⚫︎絞った配列を1つずつ、頻出数字と照らし合わせて含まれていたらcorrect+1
				correct = 0 //初期化
				for y:=0; y<int(digit); y++{
					if slices.Contains(t,digitBox[y]){ //絞った配列の1つに指定の桁数までの頻出数字が何個含まれているか確認する
						correct += 1
					}
				}
				for {  //⚫︎correctの値に応じて、箱にappendする
					if correct == int(digit){ //全てあってる場合、下記は4,3つ,それ以下あってる場合
						selectedArray3 = append(selectedArray3, t)
						break
					} else if correct == int(digit)-1 {
						mostlyCorrect = append(mostlyCorrect, t)
						break
					} else if correct == int(digit)-2{
						partlyCorrect = append(partlyCorrect, t)
						break
					} else {
						others = append(others, t)
						break
					}
				}
			}

			//⚫︎頻出度の高いものからappendする
			selectedArray3 = append(selectedArray3, mostlyCorrect...)
			selectedArray3 = append(selectedArray3, partlyCorrect...)
			selectedArray3 = append(selectedArray3, others...)
			}
			
			selectedArray2 = [][]byte{}
			firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
			addedArray = append(firstSlice, selectedArray3[0]...) //頻出順になったものの先頭を取り出して、送信できる形式にする
			WriteRead1 := WriteRead(addedArray, conn) //配列をサーバーに送信
			count = 0
			numberCount = []byte{}
			fmt.Println(h+2 ,"---------回目の送信-偶数---------")
			fmt.Println(addedArray) //送った数字
			fmt.Println(WriteRead1) //受信した結果
			answerHit := WriteRead1[10]
			answerBlow := WriteRead1[11]
			
			if WriteRead1[1] == 0{ //⚫︎正解時の処理
				break
			}

			for _,a := range selectedArray{ //⚫︎送った配列と全体の配列を比較してhit数とblow数どちらも同じものを絞る
				hit = 0
				blow = 0
				for j,b := range selectedArray3[0]{
					if a[j] == b { //絞った全体の1つの配列の1つの要素と送信した配列の1つの要素が等しいか(位置も見る)
						hit = hit + 1
					} else if slices.Contains(a,b){ //同じ位置かは考慮せずに、送信した配列の1つの要素が含まれているか
						blow = blow + 1
						// fmt.Println(a)//二次元配列から順に取り出してる配列
						// fmt.Println(b)//selectedArray[0]の中身
					} 
				}
				if answerHit == byte(hit) && answerBlow == byte(blow){ //hit数とblow数が同じ場合、箱にappend
					selectedArray2 = append(selectedArray2, a) //奇数回で使う配列に入れる
					numberCount = append(numberCount, a...) //頻出の数字をカウントするため、1次元配列に絞った配列を入れる
					count = count + 1
				} 
			}
		} else { 
            original := make([]int, len(countResult))
            selectedArray3 := [][]byte{}
            mostlyCorrect := [][]byte{}
            partlyCorrect := [][]byte{}
            others := [][]byte{}
			digitCount := []byte{}
			selectedDigit := []byte{}
			tmp0 := [][]byte{}
			tmp1 := [][]byte{}
			tmp2 := [][]byte{}
			tmp3 := [][]byte{}
 			tmp4 := [][]byte{}
			tmp5 := [][]byte{}
			tmp6 := [][]byte{}
			tmp7 := [][]byte{}
			tmp8 := [][]byte{}

			sort.Sort(sort.Reverse(sort.IntSlice(countResult)))
			if   h >= 6 {
				correctCount := 0
				for i:=1; i<10; i++{
					for _,o := range selectedArray2{
						digitCount = append(digitCount, o[i-1])
						digitCount2 := []int{
							strings.Count(string(digitCount), string(0)),
							strings.Count(string(digitCount), string(1)),
							strings.Count(string(digitCount), string(2)),
							strings.Count(string(digitCount), string(3)),
							strings.Count(string(digitCount), string(4)),
							strings.Count(string(digitCount), string(5)),
							strings.Count(string(digitCount), string(6)),
							strings.Count(string(digitCount), string(7)),
							strings.Count(string(digitCount), string(8)),
							strings.Count(string(digitCount), string(9))}

						tmp := make([]int, len(digitCount2))
						copy(tmp,digitCount2)
						sort.Sort(sort.Reverse(sort.IntSlice(digitCount2)))
						shortened := slices.Compact(digitCount2)
						for d,e:= range tmp {
							if e == shortened[0]{
								selectedDigit = append(selectedDigit, byte(d))
								break
							}
						}
						digitCount2 = []int{}
					}	
					digitCount = []byte{}
				}
				for f,g := range selectedArray2{
					for h,i := range selectedArray2[f]{
							if i == selectedDigit[h]{
								correctCount++
							}
						}	
						if correctCount == int(digit){
							tmp0 = append(tmp0, g)
							correctCount = 0
						} else if correctCount == int(digit)-1{
							tmp1 = append(tmp1, g) 
							correctCount = 0
						} else if correctCount == int(digit)-2{
							tmp2 = append(tmp2, g)
							correctCount = 0
						} else if correctCount == int(digit)-3{
							tmp3 = append(tmp3, g)
							correctCount = 0
						} else if correctCount == int(digit)-4{
							tmp4 = append(tmp4, g)
							correctCount = 0
						} else if correctCount == int(digit)-5{
							tmp5 = append(tmp5, g)
							correctCount = 0
						} else if correctCount == int(digit)-6{
							tmp6 = append(tmp6, g)
							correctCount = 0
						} else if correctCount == int(digit)-7{
							tmp7 = append(tmp7, g)
							correctCount = 0
						} else if correctCount == int(digit)-8{
							tmp8 = append(tmp8, g)
							correctCount = 0
						}
					selectedArray3 = append(selectedArray3,tmp0...)
					selectedArray3 = append(selectedArray3,tmp1...)
					selectedArray3 = append(selectedArray3,tmp2...)
					selectedArray3 = append(selectedArray3,tmp3...)
					selectedArray3 = append(selectedArray3,tmp4...)
					selectedArray3 = append(selectedArray3,tmp5...)
					selectedArray3 = append(selectedArray3,tmp6...)
					selectedArray3 = append(selectedArray3,tmp7...)
					selectedArray3 = append(selectedArray3,tmp8...)	
				}
				digitCount = []byte{}
			} else {
            copy(original,countResult)
            for x:=0; x<len(slices.Compact(countResult)); x++{
                slices.Compact(countResult)
                sort.Sort(sort.Reverse(sort.IntSlice(countResult)))
                for f,g := range original{ 
                    if g == countResult[x] {
                        digitBox = append(digitBox, byte(f))
                    } 
                }
            }
            for _,t := range selectedArray2{
                correct = 0
                for y:=0; y<int(digit); y++{
                    if slices.Contains(t,digitBox[y]){
                        correct += 1
                    }
                }
                for { 
                    if correct == int(digit){
                        selectedArray3 = append(selectedArray3, t)
                        break
                    } else if correct == int(digit)-1 {
                        mostlyCorrect = append(mostlyCorrect, t)
                        break
                    } else if correct == int(digit)-2{
                        partlyCorrect = append(partlyCorrect, t)
                        break
                    } else {
                        others = append(others, t)
                        break
                    }
                }
            }
            selectedArray3 = append(selectedArray3, mostlyCorrect...)
            selectedArray3 = append(selectedArray3, partlyCorrect...)
            selectedArray3 = append(selectedArray3, others...)
		}
            selectedArray = [][]byte{}
            firstSlice := []byte{2,0,0,0,0,0,0,0,0,0}
            addedArray := append(firstSlice, selectedArray3[0]...)
            WriteRead1 := WriteRead(addedArray, conn)
            count = 0
            numberCount =[]byte{}
            fmt.Println(h+2 ,"---------回目の送信-奇数---------")
            fmt.Println(addedArray)
            fmt.Println(WriteRead1)
            answerHit := WriteRead1[10]
            answerBlow := WriteRead1[11]
    
            if WriteRead1[1] == 0{
                break
            }

            for _,a := range selectedArray2{
                hit = 0
                blow = 0
                for j,b := range selectedArray3[0]{
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
        }
	 	fmt.Println(count,"つまで配列が絞れたよ-------------------------")	
	}
	return selectedArray 
}	

func main() {
	var digit byte
	ar := []byte{0,1,2,3,4,5,6,7,8,9} //permutation用
	
	//桁数入力
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
	permutation := permutation(ar,digit)
	//初期値を送って、絞る
	selectedArray, firstCountResult := firstCheck(permutation, conn, digit)
	//絞ったものをさらに送受信しながら何度も絞る
	check(conn, selectedArray,digit,firstCountResult)
}