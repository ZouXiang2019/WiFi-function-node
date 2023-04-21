import socket 
from threading import Thread
import multiprocessing
import time
import json
import numpy
import os
import sys
import struct

import binascii
import operator

# ADDRESS = ('192.168.0.59', 3333) 
ADDRESS = ('192.168.5.129', 3333) 
 
g_socket_server = None 
num=0
g_conn_pool = {} 

def init():
    global g_socket_server
    g_socket_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    g_socket_server.bind(ADDRESS)
    g_socket_server.listen(20)  
    print("server start，wait for client connecting...")
def message_handle(client, info, num):
    print(num)
    while True:
        try:
            buf = client.recv(70)
            t = time.time()
            f = open("CSI"+str(num)+".csv","a")
            output = ''
            for i in range(len(buf)):
                output += str(buf[i]) + (',' if (i < len(buf) - 1) else ',')
            output += str(t)
            output += ('\n')
            f.write(output)
            f.close()
        except socket.error as msg:
            print(msg)
            sys.exit(1)

if __name__ == '__main__':
    init()
    while True:
        try:
            client, info = g_socket_server.accept()
            num += 1
            print(num)
            sing_process = multiprocessing.Process(target=message_handle, args=(client, info, num))
            sing_process.daemon = True
            sing_process.start()
        except ConnectionResetError:
            pass
        except Exception as e:
            print(e)