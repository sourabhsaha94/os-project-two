#!/bin/bash
sudo rmmod keyvalue
sudo make
sudo cp keyvalue-blacklist.conf /etc/modprobe.d
sudo cp 80-keyvalue.rules /etc/udev/rules.d
sudo cp include/*.h /usr/local/include/keyvalue/
update-initramfs -u
sudo insmod keyvalue.ko
