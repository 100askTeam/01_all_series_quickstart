PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

GCCPATH=/home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/6.2.1/

spi.img : start.S  uart.c main.c my_printf.c spi.c
	$(CC) -nostdlib -g -c -o start.o start.S 
	$(CC) -nostdlib -g -c -o uart.o uart.c	
	$(CC) -nostdlib -g -c -o spi.o 	spi.c	
	$(CC) -nostdlib -g -c -o main.o main.c	
	$(CC) -nostdlib -g -c -o my_printf.o my_printf.c
	$(CC) -nostdlib -g -c -o tlc5615.o tlc5615.c
	
	$(LD) -T imx6ull.lds -g start.o uart.o  spi.o main.o my_printf.o tlc5615.o -o spi_dac.elf -lgcc -L$(GCCPATH)
	$(OBJCOPY) -O binary -S spi_dac.elf  spi_dac.bin
	# $(OBJDUMP) -D -m arm  spi_dac.elf  > spi_dac.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d spi_dac.bin spi_dac.imx
  # dd if=/dev/zero of=1k.bin bs=1024 count=1
  #	cat 1k.bin spi_dac.imx > spi_dac.img

clean:
	rm -f spi_dac.dis  spi_dac.bin spi_dac.elf spi_dac.imx spi_dac.img *.o


	
