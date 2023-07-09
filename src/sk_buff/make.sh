#! /bin/bash

cd src/ && make clean && make 
if [  ! $? ];then
    echo "编译失败"
    exit 1
fi

rmmod lyj_sk_buff

echo "rmmod lyj_sk_buff.ko $?"

insmod lyj_sk_buff.ko

echo "insmod lyj_sk_buff.ko $?"
