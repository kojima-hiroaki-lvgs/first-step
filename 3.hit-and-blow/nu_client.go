package main

import (
	"flag"
	"fmt"
	"math/rand"
	"net"
	"reflect"
	"slices"
	"strconv"
	"strings"
)

//import "slices"


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
	var result string
	var myAnswer string
	
	i := flag.String("i", "none", "ip address") //flagを使ってコマンドライン引数 : -iってオプションで実行できる、これを0.0.0.0にすると全てのネットワークインターフェースを指すからアクセスできちゃう
	flag.Parse() //コマンドラインの引数のフラグが解析され、フラグが変数にバインドされる

	//DIAL サーバーとのコネクション確立
	conn, err := net.Dial("tcp", *i + ":60000") //コマンドライン引数でipアドレスを指定して
	if err != nil{ //dialの成功確認
		fmt.Println("error: ", err)
		return
	} 
	
	fmt.Println("dial success")

	//桁数の入力
	for {
		fmt.Println("桁数を入力してください")
		fmt.Scan(&digit) //標準入力
		if digit <= 9 && digit >= 3 {
			break
		} else {
			fmt.Println("9以下で桁数を入力してください")
		}
	}

	fmt.Println(digit)
	//桁を送る
	str := strconv.Itoa(digit) //stringに変換
	data := []byte(str) //配列に桁の情報を入れる
	_, err2 := conn.Write(data)
	if (err2 != nil) {
	fmt.Println("cannot write", err2)
	} else {
	fmt.Println("digit sent")
	}

	//数値決定の<00>を受け取る
	buf := make([]byte, 1024)
    _, err3 := conn.Read(buf)
	if (err3 != nil){
		fmt.Println("cannot read", err3)
	} else {
		fmt.Println("agreement received")
	} 
	 
	//回答の数値決定の通知を受け取って、自分の回答の数値を決める
	if(string([]byte(buf)[:2]) == "00"){ //相手から00を受け取ったら
		for{
		fmt.Println("回答を入力してください")
		fmt.Scan(&myAnswer)
		if len(myAnswer) == digit { //指定の桁で回答が入力されているか
			fmt.Println(myAnswer + "に決定しました")
			break
		} else {
			fmt.Println(digit,"桁で入力してください")
		}
	}

		//回答の数値決定の通知を送信
		str := "00"
		data := []byte(str)
		_, err2 := conn.Write(data)
		if err2 != nil {
			fmt.Println("cannot write", err)
		} else {
			fmt.Println("agreement sent")
		}
		
		//クライアント側が先に予測を決定
		for i:=1; i < 100; i++{
			hitCount := 0
			blowCount := 0
			count := i
		
			for{
				fmt.Println(count,"回目の回答を入力してください")
				fmt.Scan(&result)
				if digit == len(result) { //指定した桁数で送信できているか
					break
				} else {
					fmt.Println(digit,"桁で入力してください")
				}
			}

			//決定後の送信
			str := result
			data := []byte(str) //送信する箱に標準入力した値を入れる
			_, err2 := conn.Write(data)
			if (err2 != nil) {
			fmt.Println("cannot write", err2)
			} else {
			fmt.Println("answer sent")
			}

			//自分の回答の答え合わせ
			buf1 := make([]byte, 1024)
			n1, err3 := conn.Read(buf1) //ex)不正解の場合のbufの中身 <02> 0hit2blow
					
			if (err3 != nil){
			   fmt.Println("cannot read", err3)
			} else if buf1[0] == byte(57) && buf1[1] == byte(57){ //完全一致の場合99がバイト列で来るため,9に該当する57を置いている
				fmt.Println("you win")
				return
			} else {
				HitBlow := strings.Split(string(buf1),"")[:n1] //配列にする
				fmt.Println(HitBlow[0],"hit",HitBlow[1],"blow")
				fmt.Println("review received")
			} 

			//サーバーからの回答を読み込む
			buf2 := make([]byte, 1024)
			n1, err4 := conn.Read(buf2)
			fmt.Println("answer received")
			result = string(buf2)[:n1]
			slicedA := strings.Split(myAnswer, "") //配列にする
			slicedR := strings.Split(result, "") //配列にする
			if (err4 != nil){
				fmt.Println("cannot read", err4)
				return
			} else {
		  		fmt.Println(fmt.Println("answer received"))
			} 
			
			//自分の回答と相手の回答が一致しているか
			for n:=0; n<len(result); n++ {
				if(reflect.DeepEqual(slicedA,slicedR) == true){ //完全一致
					data := []byte("99")
					_, err = conn.Write(data) 
					fmt.Println("you lose")
					return
				} else if(slicedR[n] == slicedA[n]== true){ //hit(桁のみの完全一致)
					hitCount++
				} else if(strings.Contains(myAnswer,slicedR[n])){
					blowCount++
				}
			}

			//相手にhit&blowを送る
			hit := strconv.Itoa(hitCount)
			data3 := []byte(hit)
			_, err = conn.Write(data3) 
			blow := strconv.Itoa(blowCount)
			data2 := []byte(blow)
			_, err = conn.Write(data2) 
			fmt.Println("相手は",hit,"hit",blow,"blow")
		}
	} 
	return;
}	






