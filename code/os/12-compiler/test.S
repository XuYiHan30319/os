.extern  myprintfWithoutCanshu
.global func1
.data
str0 :.string "helloworld"
str1 :.string "这是第二条语句"
str2 :.string "你好,我是xyh"
.text
func1 :
la a0,str0
call myprintfWithoutCanshu
la a0,str1
call myprintfWithoutCanshu
la a0,str2
call myprintfWithoutCanshu
ret
.end