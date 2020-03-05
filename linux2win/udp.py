from socket import *
import threading


def rcv_func():
	fd_rcv = socket(AF_INET, SOCK_DGRAM)
    local_addr = ("", 8888)
    fd_rcv.bind(local_addr)
    
    while True:
        rcv_data = fd_rcv.recvfrom(1024)
        print(rcv_data)
        
def send_func():
    fd_send = socket(AF_INET, SOCK_DGRAM)
    
    while True:
        send_info = input("待发送信息：")
        
        print(send_info)
        
rcv_thread = threading.Thread(target = rcv_func)
send_thread = threading.Thread(target = send_func)

t1.start()
t2.start()

print("the end")

'''
from socket import *
import time

fd = socket(AF_INET, SOCK_DGRAM)

send_addr = ("192.168.31.115", 8080)

for i in range(10):
	fd.sendto("haha" + str(i) + "\n", send_addr)
	time.sleep(1)

fd.close()
'''
