EXTRA_CFLAGS=-O2

KERNELDIR	?= /lib/modules/$(shell uname -r)/build
PWD			:= $(shell pwd)

obj-m := joemem.o

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

