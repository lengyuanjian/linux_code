#! /bin/bash

cd src/
make clean
make
rmmod lyj_driver
insmod lyj_driver.ko