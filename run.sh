#!/bin/bash

sudo rmmod keyvalue
sudo make -C kernel_module/
sudo make install -C kernel_module/
sudo make clean all install -C library/
sudo make clean all -C benchmark/
sudo insmod kernel_module/keyvalue.ko
benchmark/./test1