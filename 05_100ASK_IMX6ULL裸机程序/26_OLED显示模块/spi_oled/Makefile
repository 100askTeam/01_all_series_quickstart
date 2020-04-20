PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

spi_oled.img : start.S main.c spi.c spi_oled.c
	$(CC) -nostdlib -g -c -o start.o start.S 
	$(CC) -nostdlib -g -c -o spi.o 	spi.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	$(CC) -nostdlib -g -c -o spi_oled.o spi_oled.c
	
	$(LD) -T imx6ull.lds -g start.o spi.o main.o spi_oled.o -o spi_oled.elf 
	$(OBJCOPY) -O binary -S spi_oled.elf  spi_oled.bin
	# $(OBJDUMP) -D -m arm  spi_oled.elf  > spi_oled.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d spi_oled.bin spi_oled.imx
	# dd if=/dev/zero of=1k.bin bs=1024 count=1
	# cat 1k.bin spi_oled.imx > spi_oled.img

clean:
	rm -f spi_oled.dis spi_oled.bin spi_oled.elf spi_oled.imx spi_oled.img *.o


	
