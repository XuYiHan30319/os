.extern  myprintfWithoutCanshu
.global func1
.data
str0 :.string "helloworld"
str1 :.string "这是第二条语句"
.text
func1 :
la a0,str0
call myprintfWithoutCanshu
la a0,str1
call myprintfWithoutCanshu
ret
.end