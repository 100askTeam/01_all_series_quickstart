PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump
GCCDIR=$(shell which arm-linux-gnueabihf-gcc)
LIBDIR=$(subst arm-linux-gnueabihf-gcc,../,$(GCCDIR))
i2c.img : start.S  uart.c i2c.c ap3216c.c main.c my_printf.c string_utils.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o uart.o uart.c	
	$(CC) -nostdlib -g -c -o i2c.o i2c.c	
	$(CC) -nostdlib -g -c -o ap3216c.o ap3216c.c
	$(CC) -nostdlib -g -c -o main.o main.c	
	$(CC) -nostdlib -g -c -o my_printf.o my_printf.c
	$(CC) -nostdlib -g -c -o string_utils.o string_utils.c
	
	$(LD) -T imx6ull.lds -g start.o i2c.o uart.o ap3216c.o main.o my_printf.o string_utils.o -o i2c.elf -lgcc -L$(LIBDIR)lib/gcc/arm-linux-gnueabihf/6.2.1
	
	$(OBJCOPY) -O binary -S i2c.elf  i2c.bin
	$(OBJDUMP) -D -m arm  i2c.elf  > i2c.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d i2c.bin i2c.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin i2c.imx > i2c.img

clean:
	rm -f i2c.dis  i2c.bin i2c.elf i2c.imx i2c.img *.o


	
