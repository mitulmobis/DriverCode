obj-m := pcdPlatformDriver.o platformDevice.o

KDIR = /lib/modules/$(shell uname -r)/build/

host:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

help:
	make -C $(KDIR) M=$(PWD) help
