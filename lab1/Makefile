obj-m = ch_drv.o
PWD = $(shell pwd)

module:
	sudo make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules

client:
	${CC} -o test test.c

all:
	make module
	make client

clean-module:
	sudo make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean

clean-client:
	rm client

clean:
	make clean-module
	make clean-client

load:
	sudo insmod ch_drv.ko

unload:
	sudo rmmod ch_drv	
