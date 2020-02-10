PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

led.img : start.S  led.c main.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o led.o led.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	
	$(LD) -T imx6ull.lds -g start.o led.o main.o -o led.elf 
	
	$(OBJCOPY) -O binary -S led.elf  led.bin
	$(OBJDUMP) -D -m arm  led.elf  > led.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d led.bin led.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin led.imx > led.img

clean:
	rm -f led.dis  led.bin led.elf led.imx led.img *.o


	
