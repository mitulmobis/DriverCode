obj-m := nNodeDrv.o

KDIR = /lib/modules/$(shell uname -r)/build/
PWDR = $(shell pwd)

host:
	make -C $(KDIR) M=$(PWDR) modules

clean:
	make -C $(KDIR) M=$(PWDR) clean

help:
	make -C $(KDIR) M=$(PWDR) help