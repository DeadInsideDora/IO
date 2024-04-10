obj-m = lab2.o
PWD = $(shell pwd)

all::
 sudo make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules

clean:
 sudo make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean

load:
 sudo insmod lab2.ko

unload:
 sudo rmmod lab2
