package main

import (
	"fmt"
	"math/rand"
	"net"
	"reflect"
	"slices"
	"strconv"
	"strings"
)

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
	var myAnswer string
	var result string

	//LISTEN ポート開く
	ln, err := net.Listen("tcp",":60000")
	fmt.Println("listen success")
	if err != nil {
		fmt.Println("cannot listen", err)
	}

	//ACCEPT 通信接続の要求が来るまで停止
	conn, err := ln.Accept()
	fmt.Println("accept success")
	if err != nil{
		fmt.Println("cannot accept", err)
	}

	//桁を受け取る
	buf := make([]byte, 1024)
	n1, _:= conn.Read(buf)
	fmt.Println("digit received")
	lenOfAnswer,_ := strconv.Atoi(string(buf)[:n1]) //桁をintに変換
	if lenOfAnswer == 0 {
		fmt.Println("error")
		return
	}
	
	//自分の回答を決定して、数値決定の00を送信
	for{
		fmt.Println("回答を決定してください")
		fmt.Scan(&myAnswer)
		if len(myAnswer) == lenOfAnswer{
			fmt.Println(myAnswer + "に決定しました")
			break
		} else {
			fmt.Println(lenOfAnswer,"桁で入力してください")
		}
	}
	str := "00"
	data := []byte(str)
	_, err = conn.Write(data) 
	if err != nil {
		fmt.Println("cannot write", err)
	} else {
		fmt.Println("agreement sent")
	}

	//数値決定の通知を受け取る
	for{
		buf := make([]byte, 1024)
		_, err2:= conn.Read(buf)
		if(string([]byte(buf)[:2]) == "00"){
			fmt.Println("agreement received")
			break
		} else if (err2 != nil){
			fmt.Println("error")
			return
		}
	}
	
	//相手の回答を受信	
	for i:=1; i < 100; i++{
		hitCount := 0
		blowCount := 0
		count := i
		buf := make([]byte, 1024)
		n1, _:= conn.Read(buf) 
		result = string(buf)[:n1] //n1使わないとエラー出るから[:n1]にしてる、なくてもいい
		slicedA := strings.Split(myAnswer, "") //自分の回答を配列にする
		slicedR := strings.Split(result, "") //空いてから受け取った数字を配列にする
		fmt.Println("answer received")
	
		//自分の回答と相手の回答が一致しているか確認して、正解の場合は結果を送信
		for n:=0; n<len(result); n++ {
			if(reflect.DeepEqual(slicedA,slicedR) == true){ //配列の比較、完全一致の場合
				data := []byte("99")  //一致してる場合は99を送る 
				_, err = conn.Write(data) 
				fmt.Println("you lose")
				return;
			} else if(slicedR[n] == slicedA[n]== true){ //hit(桁のみの完全一致) 位置を見てる
				hitCount++
			} else if(strings.Contains(result,slicedA[n])){ //blow  含まれているか
				blowCount++
			} 
		}	

		//相手にhit&blowを送る
		hit := strconv.Itoa(hitCount) //stringに変換
		data := []byte(hit)
		_, err = conn.Write(data) 
		blow := strconv.Itoa(blowCount) //stringに変換
		data2 := []byte(blow)
		_, err = conn.Write(data2) 
		fmt.Println("相手は",hit,"hit",blow,"blow")
		
		//サーバー側が回答
		for {
			fmt.Println(count,"回目の回答を入力してください")
			fmt.Scan(&result)
			resultArray := strings.Split(result, "") //配列に入れる
			if len(resultArray) == lenOfAnswer { //入力したものが指定の桁数か確認
				break
			} else {
				fmt.Println(lenOfAnswer,"桁で入力してください")
			}
		}
		str := result
		data = []byte(str)
		_, err = conn.Write(data) 

		////受信した結果をもとにの答え合わせ
		buf5 := make([]byte, 1024)
		_, err7 := conn.Read(buf5) //
		if (err7 != nil){
			fmt.Println("cannot read", err7)
			return
		} else if buf5[0] == byte(57) && buf5[1] == byte(57){
			fmt.Println("you win")
			return
		} else {
			HitBlow := strings.Split(string(buf5),"")
			fmt.Println(HitBlow[0],"hit",HitBlow[1],"blow")
			fmt.Println("review received")
		} 
	}
}	






