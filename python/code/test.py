from threading import Thread
import time

g_cnt = 0
g_flag = 0

def work1():
    global g_cnt
    global g_flag
    
    while 0 == g_flag:
        pass
        
    for i in range(1000000):
        g_cnt += 1
            
    g_flag = 0

def work2():
    global g_cnt
    global g_flag
    
    while 1 == g_flag:
        pass
    
    for i in range(1000000):
        g_cnt += 1
    
    g_flag = 1

p1 = Thread(target = work1)
p2 = Thread(target = work2)

p1.start()
p2.start()

p1.join()
p2.join()

print("AAAA")
print("g_cnt = %d"%g_cnt)

'''
from multiprocessing import Process
import time

def func1():
    for tmp in range(5):
        print("------1")
        time.sleep(1)
    
def func2():
    for tmp in range(3):
        print("------2")
        time.sleep(1)
    
if __name__ == "__main__":
    p1 = Process(target=func1)
    p2 = Process(target=func2)
    
    p1.start()
    p2.start()
    
    print("main ----1")
    p1.join()               #等到p1进程执行完毕，父进程继续执行
    print("main ----2")
    
    print("main done")
'''

'''
from multiprocessing import Pool
import os
import random
import time

def worker():
    #for i in range(5):
    #    print("process=%d, pid=%d"%(os.getpid(), num))
    #    time.sleep(1)
    print("BBBB")

print("AAAA")

pool = Pool(3)

pool.close()
pool.join()
'''

'''
def w1(func):
    print("装饰器1")
    def inner():
        print("装饰器1执行")
        func()
    return inner

def w2(func):
    print("装饰器2")
    def inner():
        print("装饰器2执行")
        func()
    return inner

@w1                     #f1 = w1(f1)
@w2                     #f1 = w2(f1)    -- 先装饰(替换)w2
def f1():
    print("f1执行")
    
#f1 = w1(f1)
#f1 = w2(f1)

f1()
'''


'''
# 测试私有属性重写 -- error
# AttributeError: 'B' object has no attribute '_A__name'
# 子类调用父类方法get_name()，实际执行 return self._A__name
# B重写了父类的__init__方法，父类中该方法定义_A__name

class A:
    #__name = "zhangsan"

    def __init__(self):
        self.__name = "zhangsan"
        print("A init() exe")
    
    def get_name(self):
        return self.__name      # self._A_name not self.__name

class B(A):
    def __init__(self):
        print("B init() exe")
        self.__name = "lisi"

    #def get_name(self):
    #    return self.__name

    pass
    
a = A()
b = B()

print(dir(a))
print(dir(b))

print(a.get_name())
print(b.get_name())
'''

#批量修改文件名
#考虑到文件名中可能有'.'的情况，使用直接'.'替换为'[备份].'的方式不满足
#从文件右边开始查找到第一个'.'位置
'''
import os

dir_name = "test_dir"
#os.chdir(os.getcwd() + "\\" + dir_name)
os.chdir(os.getcwd() + "/" + dir_name)
#os.chdir(os.getcwd() + "\" + dir_name)      # os.chdir(os.getcwd() + "\" + dir_name)


print(os.getcwd())

a = os.listdir()
index = 20

for tmp in a:
    print(tmp)

for tmp in a:
    print(tmp)
    
    pos = tmp.rfind(".")
    new_name = str(index) + tmp[pos:]
    index += 1
    print(new_name)
    
    os.rename(tmp, new_name)
'''

#备份文件名
'''
file_name = input("输入需要备份文件名: ")

fd = open(file_name, "r")

content = fd.read()

new_file_name = file_name.split(".")
new_file_name = new_file_name[0] + "备份." + new_file_name[1]

fd1 = open(new_file_name, "w")

fd1.write(content)
fd1.close()

print(new_file_name + "内容: ")
fd1 = open(new_file_name, "r")
content = fd1.read()
print(content)

fd.close()
fd1.close()
'''

'''
a = input("输入a的值: ")
a = int(a)
if 10 == a:
    print(a)
elif 100 == a:
    print("a = 100", a, "%d" %(a))
else:
    print("不相等")
    
while a > 0:
    print("a ", end = "")
    a -= 1
print("")
'''

'''
a = 10
if a > 10:
	print("aaaa")
else:
	print("bbbb")
	
i = 0
j = 0
while i < 10:
	print(i)
	i += 1
'''