$@ $1,$2,$3...
$* $1$2$3...
$0 脚本程序名称
$? 上一个命令的返回码
$# 参数个数

stat filename|grep Modify|awk '{print $2}'|sed s/-//g #获取文件的修改日期
