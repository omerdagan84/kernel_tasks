#################################################################
#	kerneltask - task18 - Makefile								#
#																#
#	Author: Omer Dagan											#
#	Date: 02.01.17												#
#	Based on: http://www.tldp.org/LDP/lkmpg/2.6/html/x181.html	#											
#################################################################


obj-m += task18.o
CFLAGS_task18.o := -DDEBUG
all:
		make  -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
