PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

uart.img : start.S  uart.c main.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o uart.o uart.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	
	$(LD) -T imx6ull.lds -g start.o uart.o main.o -o uart.elf 
	
	$(OBJCOPY) -O binary -S uart.elf  uart.bin
	$(OBJDUMP) -D -m arm  uart.elf  > uart.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d uart.bin uart.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin uart.imx > uart.img

clean:
	rm -f uart.dis  uart.bin uart.elf uart.imx uart.img *.o


	
