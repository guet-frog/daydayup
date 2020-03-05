from socket import *

def main():
    local_addr = ("", 8888)
    
    fd = socket(AF_INET, SOCK_DGRAM)
    
    fd.bind(local_addr)
    
    while True:
        info = fd.recvfrom(1024)        # buff size
        #print(info)
        dst_addr = info[1]
        send_data = info[0] + "--sendbank".encode("utf8")
        fd.sendto(send_data, dst_addr)
        
if "__main__" == __name__:
    main()

