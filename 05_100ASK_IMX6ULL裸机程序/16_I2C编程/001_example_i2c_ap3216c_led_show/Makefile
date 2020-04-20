PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

i2c.img : start.S  i2c.c ap3216c.c main.c led.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o i2c.o i2c.c	
	$(CC) -nostdlib -g -c -o ap3216c.o ap3216c.c
	$(CC) -nostdlib -g -c -o main.o main.c	
	$(CC) -nostdlib -g -c -o led.o led.c
	
	$(LD) -T imx6ull.lds -g start.o i2c.o ap3216c.o main.o led.o -o i2c.elf 
	
	$(OBJCOPY) -O binary -S i2c.elf  i2c.bin
	$(OBJDUMP) -D -m arm  i2c.elf  > i2c.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d i2c.bin i2c.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin i2c.imx > i2c.img

clean:
	rm -f i2c.dis  i2c.bin i2c.elf i2c.imx i2c.img *.o


	
