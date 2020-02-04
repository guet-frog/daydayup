
vi /etc/apt/sources.list
sudo apt-get update

人工智能、爬虫、web服务器

python进入交互模式，测试基本语法
exit()退出

ipython支持linux命令，支持自动补全
ipython3

python3将输入作为字符串	a = 'abc'				a = 1+4 	# a = '1+4'
python2将输入作为代码执行	a = abc #未定义abc		a = 1+4 	# a = 5

python2中使用raw_input()

强制类型转换 age_num = age(age)

"="*50

print("%d %d %d"%(name,age,addr))
print多一个%

在命令行模式下敲命令python，就看到类似如下的一堆文本输出，然后就进入到Python交互模式，它的提示符是>>>

print(100 + 200 + 300)
print('100 + 200 =', 100 + 200)
print(name)

用Python开发程序，完全可以一边在文本编辑器里写代码，一边开一个交互式命令窗口，在写代码的过程中，把部分代码粘到命令行去验证，事半功倍！

当语句以冒号:结尾时，缩进的语句视为代码块

字符串是以单引号'或双引号"括起来的任意文本

没有do while
switch
++ --

print("", end="")

len()

执行加法，python会自动检测类型 -- 组成字符串的两种方式

name[len(name)-1]
name[-1]
name[-2]

name[2:5]
name[2:-3]	// 取第二个元素到倒数第三个元素
name[2:]	// 取第二个元素到最后

name[2:-1:2]	// 取第二个元素到倒数第一个元素，每2个取一个

name[]
name[::-1]		// 倒序输出
print(name)		// 正序输出，不切片

字符串可以使用切片，列表也可以使用切片
列表可以存储不同类型数组
列表一般用来存储相同类型元素

list.append(para)	// 添加到最后，作为一个元素添加
list.insert(index, para)	// 添加到任意位置

list.extend(list)	// 合并两个元组

列表增、删、改、查
增: append() insert() extend()

删: pop()		// 删除最后一个
	remove()	// 根据内容删除第一个
	del list[0]	// 根据内容删除
	
修改: list[0] = new value

查: if "xxx" in names:
		print()
	if "xxx" not in names:
		print()

	if num == 1:
		pass		// 只用写一次
		
字典的增、删、改、查
infor = {键:值, 键:值}
info = {"name":"班长", "addr":"山东"}

del info["name"]		// 删
info["name"] = "班长"	// 增、修改
info.get("name")		// 查

if "qq" in info.keys()
info.values()
info.items()

		
for遍历比较方便
while需要处理长度、下标

for else 和 break
		
元组类似于const

a = (11, 22)
c,d = a    		// 拆包

for a,b in info.items():
	print("key= value=")

return a, b, c  相当于 return (a, b, c)

// ------------------------------------
def modify_wendu():
	global wendu
	wendu = 33

	// 列表、字典可以不用添加global直接使用
	
// ------------------------------------
a = 100

def test():
	print(a)
	print(b)

b = 200

test()		// ok -- 100 200
// ------------------------------------

help(print)







		