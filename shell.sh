$@ $1,$2,$3...
$* $1$2$3...
$0 脚本程序名称
$? 上一个命令的返回码
$# 参数个数
#stat filename | grep Modify => Modify: 2019-09-18 10:05:55.526576976 +000
stat filename | grep Modify | awk '{print $2}'|sed s/-//g #20190918
stat filename | grep Modify | awk '{print $2 $3}' | cut -d"." -f1 | sed -e 's/-//g' -e 's/://g' #20190918100555
