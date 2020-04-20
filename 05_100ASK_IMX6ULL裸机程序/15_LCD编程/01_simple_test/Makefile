PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

simple_test.img : start.S  imx6ull_con.c main.c lcd_manager.c lcd_7_0.c lcd_controller_manager.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o lcd_controller_manager.o lcd_controller_manager.c	
	$(CC) -nostdlib -g -c -o lcd_manager.o lcd_manager.c
	$(CC) -nostdlib -g -c -o lcd_7_0.o lcd_7_0.c
	$(CC) -nostdlib -g -c -o imx6ull_con.o imx6ull_con.c	
	$(CC) -nostdlib -g -c -o main.o main.c
	
	$(LD) -T imx6ull.lds -g start.o main.o lcd_controller_manager.o lcd_manager.o lcd_7_0.o imx6ull_con.o -o simple_test.elf
	
	$(OBJCOPY) -O binary -S simple_test.elf  simple_test.bin
	$(OBJDUMP) -D -m arm  simple_test.elf  > simple_test.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d simple_test.bin simple_test.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin simple_test.imx > simple_test.img

clean:
	rm -f simple_test.dis  simple_test.bin simple_test.elf simple_test.imx simple_test.img *.o


	
