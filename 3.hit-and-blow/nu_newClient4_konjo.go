package main

import (
	"fmt"
	"net"
)

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
		//fmt.Println(answerSlice2)
			if buf[1] == 0{
				fmt.Println("correct")
			}
	}
	return buf
}

func addAnswer(digitSlice []byte, digit byte, conn net.Conn) []byte {

	data2 := []byte{0x02,0,0,0,0,0,0,0,0,0}
	answerSlice2 := data2[0:10]

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
					a := ReadWrite(answerSlice2, conn)	
					if a[1] == byte(0) {
						break kojima
					}
				}

				for l := 0; l < 10; l++{
					
					if  l == i || l == j || l == k {
						continue
						} else if  digit == 4  {
						
						answerSlice2 = append(answerSlice2[:10],byte(i))
						answerSlice2 = append(answerSlice2[:],byte(j))
						answerSlice2 = append(answerSlice2[:],byte(k))
						answerSlice2 = append(answerSlice2[:],byte(l))
						a := ReadWrite(answerSlice2, conn)	
						if a[1] == byte(0) {
							break kojima
						} 
					}
					for m := 0; m < 10; m++{
						
						if  m == i || m == j || m == k || m == l {
							continue
							} else if  digit == 5  {
							
							answerSlice2 = append(answerSlice2[:10],byte(i))
							answerSlice2 = append(answerSlice2[:],byte(j))
							answerSlice2 = append(answerSlice2[:],byte(k))
							answerSlice2 = append(answerSlice2[:],byte(l))
							answerSlice2 = append(answerSlice2[:],byte(m))
							a := ReadWrite(answerSlice2, conn)	
							if a[1] == byte(0) {
								break kojima
							} 
						}
						for n := 0; n < 10; n++{
							if  n == i || n == j || n == k || n == l || n == m{
								continue
								} else if  digit == 6  {
								
								answerSlice2 = append(answerSlice2[:10],byte(i))
								answerSlice2 = append(answerSlice2[:],byte(j))
								answerSlice2 = append(answerSlice2[:],byte(k))
								answerSlice2 = append(answerSlice2[:],byte(l))
								answerSlice2 = append(answerSlice2[:],byte(m))
								answerSlice2 = append(answerSlice2[:],byte(n))
								a := ReadWrite(answerSlice2, conn)	
								if a[1] == byte(0) {
									break kojima
								} 
							}
							for o := 0; o < 10; o++{
								if o == i || o == j || o == k || o == l || o == m || o == n{
									continue
									} else if  digit == 7  {
									
									answerSlice2 = append(answerSlice2[:10],byte(i))
									answerSlice2 = append(answerSlice2[:],byte(j))
									answerSlice2 = append(answerSlice2[:],byte(k))
									answerSlice2 = append(answerSlice2[:],byte(l))
									answerSlice2 = append(answerSlice2[:],byte(m))
									answerSlice2 = append(answerSlice2[:],byte(n))
									answerSlice2 = append(answerSlice2[:],byte(o))
									a := ReadWrite(answerSlice2, conn)	
									if a[1] == byte(0) {
										break kojima
									} 
								}
								for p := 0; p < 10; p++{
									if p == i || p == j || p == k || p == l || p == m || p == n || p == o{
										continue
										} else if  digit == 8  {
										
										answerSlice2 = append(answerSlice2[:10],byte(i))
										answerSlice2 = append(answerSlice2[:],byte(j))
										answerSlice2 = append(answerSlice2[:],byte(k))
										answerSlice2 = append(answerSlice2[:],byte(l))
										answerSlice2 = append(answerSlice2[:],byte(m))
										answerSlice2 = append(answerSlice2[:],byte(n))
										answerSlice2 = append(answerSlice2[:],byte(o))
										answerSlice2 = append(answerSlice2[:],byte(p))
										a := ReadWrite(answerSlice2, conn)	
										if a[1] == byte(0) {
											break kojima
										} 
									}
									for q := 0; q < 10; q++{
										if q == i || q == j || q == k || q == l || q == m || q == n || q == o || q == p {
											continue
											} else if  digit == 9  {
											
											answerSlice2 = append(answerSlice2[:10],byte(i))
											answerSlice2 = append(answerSlice2[:],byte(j))
											answerSlice2 = append(answerSlice2[:],byte(k))
											answerSlice2 = append(answerSlice2[:],byte(l))
											answerSlice2 = append(answerSlice2[:],byte(m))
											answerSlice2 = append(answerSlice2[:],byte(n))
											answerSlice2 = append(answerSlice2[:],byte(o))
											answerSlice2 = append(answerSlice2[:],byte(p))
											answerSlice2 = append(answerSlice2[:],byte(q))
											a := ReadWrite(answerSlice2, conn)	
											fmt.Println(answerSlice2)
											if a[1] == byte(0) {
												break kojima
											} 
										}
									}
								}
							}
						}
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


  

	