
'''
#ThreadLocal变量虽然是全局变量，但每个线程都只能读写自己线程的独立副本
#ThreadLocal解决了参数在一个线程中各个函数之间互相传递的问题
import threading
import time

#g_name = None
g_name = threading.local()  # g_name虽然是全局属性，但是对于线程是安全的

def process_thread(name):
    #global g_name
    #g_name = name
    
    g_name.name = name      #对象动态添加属性，也可以被限制
    
    time.sleep(1)
    #print(g_name)
    print(g_name.name)
    
t1 = threading.Thread(target=process_thread, args=("aa",))
t2 = threading.Thread(target=process_thread, args=("bb",))

t1.start()
t2.start()

print("end")
'''

'''
def main():
    a = 100
    func()
    
if "__main__" == __name__:
    main()
'''

'''
from multiprocessing import Process
from multiprocessing import Queue
#from queue import Queue            # 使用在进程中，达不到预期的效果
import time

#q = Queue(5)                       # 多进程，需要注意全局变量，不能用于信息传递

def process1(q):
    for i in range(10):
        q.put(i)
        print("AAAA - %d", q.size())
        time.sleep(1)
        
    print("p1 end")

def process2(q):
    for i in range(5):
        print(q.qsize())
        a = q.get(True)
        #print(q.get())
        print("BBBB")
        time.sleep(3)

if "__main__" == __name__:
    q = Queue(10)
    
    p1 = Process(target = process1, args = (q, ))
    p2 = Process(target = process2, args = (q, ))
    
    p1.start()
    p2.start()
    
    print("the end")
'''

'''
from multiprocessing import Process
import os
import time

def func():
    for i in range(10):
        print("AAAA")
        time.sleep(1)
        
def func1():
    for i in range(5):
        print("BBBB")
        time.sleep(1)

if __name__ == "__main__":
    p1 = Process(target = func)
    p2 = Process(target = func1)
    
    p1.start()
    p1.join()
    
    p2.start()
    
    #p2.join()
    
    print("the end")
'''

'''
import threading 

g_info = threading.local()

def func1():
    std = g_info.name
    print("thread = %s, %s"%(threading.current_thread().name, std))
    
def thread(name):
    g_info.name = name
    func1()
    
t1 = threading.Thread(target = thread, args=("zhangsan", ), name = "test1")
t2 = threading.Thread(target = thread, args=("lisi", ))

t1.start()
t2.start()

t1.join()
t2.join()

print("the end")
'''

'''
from threading import Thread, Lock, enumerate
import time

def func1():
    for i in range(8):
        mutex1.acquire()
        #mutex1.acquire(blocking = True, timeout = 5)       # 超时机制
        print("AAAA")
        mutex1.release()
        time.sleep(1)

def func2():
    for i in range(5):
        mutex1.acquire()
        #time.sleep(2)
        print("BBBB")
        mutex1.release()
        time.sleep(2)

mutex1 = Lock()
mutex2 = Lock()

p1 = Thread(target = func1)
p1.start()

p2 = Thread(target = func2)
p2.start()

p1.join()

while True:
    print("the end")
    print("current thread num = %d"%len(enumerate()))
    time.sleep(1)
'''

#两个线程对全局变量进行互斥访问
'''
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

'''
#windows不能运行
from multiprocessing import Process
import os

def func():
    for i in range(3):
        print("AAAA")
        
p = Process(target=func)
p.start()
p.join()
'''

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