import os
import time
import paramiko
import hashlib
import datetime

all_size=0

def calc_md5(file_path):
    """
    计算文件的MD5码
    :param file_path: 文件路径
    :return: 文件的MD5码
    """
    md5 = hashlib.md5()
    with open(file_path, 'rb') as f:
        while True:
            data = f.read(1024)
            if not data:
                break
            md5.update(data)
    return md5.hexdigest()

def check_and_copy_files(file_list, ssh_host, ssh_port, ssh_username, ssh_password, ssh_remote_path):
    #创建实例
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    # 连接远程服务器并进行认证
    client.connect(hostname=ssh_host, username=ssh_username, password=ssh_password,port=ssh_port)
    sftp = client.open_sftp()
    file_md5={}
    for file_name in file_list:
        sftp.put(file_name, f"{ssh_remote_path}/{os.path.basename(file_name)}")
        file_md5[file_name]=calc_md5(file_name)
        global all_size
        all_size += os.path.getsize(file_name)
    print(f"File to {ssh_remote_path} ")
    while True:
        is_diff = False
        for file_name in file_list:
            # 获取文件的MD5码
            md5 = calc_md5(file_name)
            # 间隔1秒钟检查一次是否有文件改动
            if md5 != file_md5[file_name]:
                # 如果文件改动，则上传文件到远程服务器
                sftp.put(file_name, f"{ssh_remote_path}/{os.path.basename(file_name)}")
                file_md5[file_name] = md5
                all_size += os.path.getsize(file_name)
                print(f"File {file_name}:{os.path.getsize(file_name)} -> {ssh_remote_path}/{os.path.basename(file_name)}")
                is_diff = True
        if is_diff:
            print(f"{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')} size[{all_size}]")
        time.sleep(2.0)
    sftp.close()
    transport.close()

if __name__ == '__main__':
    # 程序入口
    file_list = ["ring_buff.cpp", "ring_buff.h", "Makefile", "main.cpp"]  # 传递文件列表
    ssh_host = "10.3.71.91"  # 远程Linux服务器IP
    ssh_port = 9526  # 默认端口
    ssh_username = "lyj"  # 远程服务器登录账号
    ssh_password = "lyj123"  # 远程服务器登录密码
    ssh_remote_path = "/home/lyj/socket/buff"  # 远程服务器文件上传路径
    check_and_copy_files(file_list, ssh_host, ssh_port, ssh_username, ssh_password, ssh_remote_path)
