SHELL := /bin/bash
KERNELVERSION  ?= $(shell uname --kernel-release)
KSRC := /lib/modules/$(KERNELVERSION)/build

obj-m := lenovo_legion.o 
lenovo_legion-objs := legion-dkms.o legion-wmi-other.o legion-hwmon.o legion-sysfs.o legion-wmi-gamezone.o legion-ec.o legion-wmi.o legion-fancurve.o legion-wmi-driver.o legion-pprof.o

all:
	$(MAKE) -C $(KSRC) M=$(shell pwd) modules

allWarn:
	$(MAKE) -C $(KSRC) M=$(shell pwd) KCFLAGS=-W modules

clean:
	$(MAKE) -C $(KSRC) M=$(shell pwd) clean

dkms: all