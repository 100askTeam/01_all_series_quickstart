PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

gpio_motor.img : start.S  motor.c main.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o motor.o motor.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	
	$(LD) -T imx6ull.lds -g start.o motor.o main.o -o gpio_motor.elf 
	
	$(OBJCOPY) -O binary -S gpio_motor.elf  gpio_motor.bin
	$(OBJDUMP) -D -m arm  gpio_motor.elf  > gpio_motor.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d gpio_motor.bin gpio_motor.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin gpio_motor.imx > gpio_motor.img

clean:
	rm -f *.dis  *.bin *.elf *.imx *.img *.o


	
