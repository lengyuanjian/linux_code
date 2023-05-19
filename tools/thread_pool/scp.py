import os
import paramiko

local_path = 'D:\\vs_2017\\linux\\socket\\socket_02\\'
remote_path = '/home/lengyuanjian/socket/01/'
remote_path = '/home/lyj/socket/01/'
hostname = '192.168.118.141'
hostname = '485069iu73.wicp.vip'
username = 'lengyuanjian'
username = 'lyj'
password = 'lyj123'

all_file =['main.cpp', 'Makefile', 'scp.py', 'thread_pool.cpp', 'thread_pool.h']

# 创建SSH客户端对象
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(hostname=hostname, username=username, password=password,port=38442)

# 将本地文件拷贝到远程主机
sftp = ssh.open_sftp()
for file_one in all_file:
    try:
        sftp.put(local_path +  file_one, remote_path +  file_one)
        print("\t" + local_path +  file_one + "\t-> " + remote_path +  file_one)
    except Exception as err:
        print(err)
sftp.close()

# 关闭SSH连接
ssh.close()
