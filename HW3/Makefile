
PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(KVERSION)/build

MODULE_NAME := kfetch_mod_312555008
obj-m := $(MODULE_NAME).o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

load:
	sudo insmod $(MODULE_NAME).ko
unload:
	sudo rmmod $(MODULE_NAME).ko
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean


