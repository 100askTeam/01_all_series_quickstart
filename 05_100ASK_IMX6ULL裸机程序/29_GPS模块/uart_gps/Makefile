PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

GCCPATH=/home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/6.2.1/


uart_gps.img : start.S  uart.c main.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o uart.o uart.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	
	$(LD) -T imx6ull.lds -g start.o uart.o main.o -o uart_gps.elf -lgcc -L$(GCCPATH)
	
	$(OBJCOPY) -O binary -S uart_gps.elf  uart_gps.bin
	# $(OBJDUMP) -D -m arm  uart_gps.elf  > uart_gps.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d uart_gps.bin uart_gps.imx
	
	# dd if=/dev/zero of=1k.bin bs=1024 count=1
	# cat 1k.bin uart_gps.imx > uart_gps.img

clean:
	rm -f uart_gps.dis  uart_gps.bin uart_gps.elf uart_gps.imx uart.img *.o


	
