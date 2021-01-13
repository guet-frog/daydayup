
cygwin

git bash底层使用cygwin

a=1
a="hello world"

echo $a
echo ${a}

echo $ab
echo ${a}b

echo $		// 通过tab自动补全 查看系统变量
echo $PWD
echo $USER
echo $HOME
echo $PATH

array=(1 2 3 4 5)			// a=3 -> (3 2 3 4 5)
echo ${array[*]}
echo ${#array[*]}

""特殊作用
echo "hello ${array[*]} world"
echo 'hello ${array[*]} world'

echo "hello world\n"
echo -e "hello world\n"

((a=a+1))

变量类型
字符串
数字		// 变量是数字类型，可以通过(())进行运算 -- shell无浮点数计算
布尔

echo ${s:0:5}		// awk切片
echo ${#s}			// 计算长度 -- ${#array[*]}

echo ${s/testerhome/hogwarts}	替换字符串
echo ${s/l/_}
echo ${s//l/_}		替换字符串所有的l

echo 123
echo $?			// 每条命令的状态返回码

判断
算术判断 -eq -ne -gt -ge -lt -le
[ 2 -eq 2 ]		// 注意空格
echo $?
[ 3 -eq 2 ]
echo $?

((10>=8))		// 除了进行算术运算，也可以算术比较

字符串比较
[ "$a" = "hello world" ]		// 精准匹配

[-z "$b"]		// 判断为空
[-n "$b"]
echo ${#a}	// 长度为0 判断为空

