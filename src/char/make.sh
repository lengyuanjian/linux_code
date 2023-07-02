#! /bin/bash

cd src/ && make clean && make 
if [  ! $? ];then
    echo "编译失败"
    exit 1
fi

rmmod lyj_driver  

echo "rmmod lyj_driver $?"

insmod lyj_driver.ko

echo "insmod lyj_driver.ko $?"
