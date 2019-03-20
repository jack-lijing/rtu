#!/usr/bin/python
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = socket.gethostbyname('jack60')
port = 5901
print(host);
s.bind((host,port))

s.listen(5)
while True:
    c, addr = s.accept()
    print("连接地址:%s"%str(addr))
    data = c.recv(3);
    print("%s\n",data);
    c.send("ffff012122345".encode('utf-8'))
