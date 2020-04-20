PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

my_printf.img : start.S  uart.c main.c my_printf.c string_utils.c
	$(CC) -nostdlib -g -c -o start.o start.S 
	$(CC) -nostdlib -g -c -o uart.o uart.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	$(CC) -nostdlib -g -c -o my_printf.o my_printf.c
	$(CC) -nostdlib -g -c -o string_utils.o string_utils.c
	
	$(LD) -T imx6ull.lds -g start.o uart.o main.o string_utils.o my_printf.o -o string_utils.elf -lgcc -L/home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/6.2.1
	
	$(OBJCOPY) -O binary -S string_utils.elf  string_utils.bin
	$(OBJDUMP) -D -m arm  string_utils.elf  > string_utils.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d string_utils.bin string_utils.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin string_utils.imx > string_utils.img

clean:
	rm -f string_utils.dis  string_utils.bin string_utils.elf string_utils.imx string_utils.img *.o


	
