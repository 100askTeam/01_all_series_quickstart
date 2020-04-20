PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall
CPPFLAGS   	:= -nostdinc -fno-builtin -I$(INCLUDEDIR)
LDFLAGS         := -L /home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/6.2.1/ -lgcc
objs :=  start.o main.o led.o uart.o eabi_compat.o my_printf.o gic.o epit.o

TARGET := timer

$(TARGET).img : $(objs)
	$(LD) -T imx6ull.lds -o $(TARGET).elf $^ $(LDFLAGS)
	$(OBJCOPY) -O binary -S $(TARGET).elf  $(TARGET).bin
	$(OBJDUMP) -D -m arm  $(TARGET).elf  > $(TARGET).dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d $(TARGET).bin $(TARGET).imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin $(TARGET).imx > $(TARGET).img

%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET).dis  $(TARGET).bin $(TARGET).elf $(TARGET).imx $(TARGET).img *.o


	
