$@ $1,$2,$3...
$* $1$2$3...
$0 脚本程序名称
$? 上一个命令的返回码
$# 参数个数

if test expr
then
  statement
fi
可以写作(所有token之间都要有空格)
if test expr ; then
  statement
fi

echo something | sed -e 's/curr/after/g'  #表示用after替换something中curr

function myfunc(){
  $1 表示参数一
  $2 表示参数二
}
#函数必须先定义才能使用
myfunc hello# 表示调用函数，hello是参数

echo ^| #输出|，^表示去掉|的管道特殊含义，相当于转义用的\

#stat filename | grep Modify => Modify: 2019-09-18 10:05:55.526576976 +000
stat filename | grep Modify | awk '{print $2}'|sed s/-//g #20190918
stat filename | grep Modify | awk '{print $2 $3}' | cut -d"." -f1 | sed -e 's/-//g' -e 's/://g' #20190918100555
1、错误方法举例 
a) 
var=1+1 
echo $var 
输出的结果是1+1，悲剧，呵呵 

b) 
var=1 
var=$var+1 
echo $var 
输出结果是1+1，依然悲剧，呵呵 

2、正确方法 
1)使用let 
var=1 
let "var+=1" 
echo $var 
输出结果为2，这次没有悲剧 
注意： 
a)经我测试let几乎支持所有的运算符，在网上看到一篇文章说“let不支持++、--和逗号、(、)”,但经我测试自加、自减、以及括号的优先级都得到了很好的支持 
b)方幂运算应使用“**” 
c)参数在表达式中直接访问，不必加$ 
d)一般情况下算数表达式可以不加双引号，但是若表达式中有bash中的关键字则需加上 
e)let后的表达式只能进行整数运算 

2)使用(()) 
var=1 
((var+=1)) 
echo $var 
输出结果为2 
注意： 
(())的使用方法与let完全相同 

3)使用$[] 
var=1 
var=$[$var+1] 
echo $var 
输出结果位2 
注意： 
a)$[]将中括号内的表达式作为数学运算先计算结果再输出 
b)对$[]中的变量进行访问时前面需要加$ 
c)$[]支持的运算符与let相同，但也只支持整数运算 

4)使用expr 
var=1 
var=`expr $var + 1` 
echo $var 
输出结果为2 
注意： 
a)expr后的表达式个符号间需用空格隔开 
b)expr支持的操作符有： |、&、<、<=、=、!=、>=、>、+、-、*、/、% 
c)expr支持的操作符中所在使用时需用\进行转义的有：|、&、<、<=、>=、>、* 
e)expr同样只支持整数运算 

5)使用bc(可以进行浮点数计算) 
var=1 
var=`echo "$var+1"|bc` 
echo $var 
输出结果为2 
介绍： 
bc是linux下的一个简单计算器，支持浮点数计算，在命令行下输入bc即进入计算器程序，而我们想在程序中直接进行浮点数计算时，利用一个简单的管道即可解决问题。 
注意： 
1)经我测试bc支持除位操作运算符之外的所有运算符。 
2)bc中要使用scale进行精度设置 
3)浮点数计算实例 
var=3.14 
var=`echo "scale=2;$var*3"|bc` 
echo $var 
输出结果为9.42 

6)使用awk(可已进行浮点数计算) 
var=1 
var=`echo "$var 1"|awk '{printf("%g",$1*$2)}'` 
echo $var 
输出结果为2 
介绍： 
awk是一种文本处理工具，同时也是一种程序设计语言，作为一种程序设计语言，awk支持多种运算，而我们可以利用awk来进行浮点数计算，和上面bc一样，通过一个简单的管道，我们便可在程序中直接调用awk进行浮点数计算。 
注意： 
1)awk支持除微操作运算符之外的所有运算符 
2)awk内置有log、sqr、cos、sin等等函数 
3)浮点数计算实例 
var=3.14 
var=`echo "$var 2"|awk '{printf("%g",sin($1/$2))}'` 
echo $var 
输出结果为1 
