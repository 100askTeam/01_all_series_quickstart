PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

font_test.img : start.S  imx6ull_con.c main.c font.c font_8x16.c framebuffer.c geometry.c lcd_manager.c lcd_7_0.c lcd_controller_manager.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o lcd_controller_manager.o lcd_controller_manager.c	
	$(CC) -nostdlib -g -c -o lcd_manager.o lcd_manager.c
	$(CC) -nostdlib -g -c -o lcd_7_0.o lcd_7_0.c
	$(CC) -nostdlib -g -c -o imx6ull_con.o imx6ull_con.c	
	$(CC) -nostdlib -g -c -o main.o main.c
	$(CC) -nostdlib -g -c -o framebuffer.o framebuffer.c	
	$(CC) -nostdlib -g -c -o geometry.o geometry.c
	$(CC) -nostdlib -g -c -o font.o font.c
	$(CC) -nostdlib -g -c -o font_8x16.o font_8x16.c
	
	$(LD) -T imx6ull.lds -g start.o main.o font.o font_8x16.o geometry.o framebuffer.o lcd_controller_manager.o lcd_manager.o lcd_7_0.o imx6ull_con.o -o font_test.elf
	
	$(OBJCOPY) -O binary -S font_test.elf  font_test.bin
	$(OBJDUMP) -D -m arm  font_test.elf  > font_test.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d font_test.bin font_test.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin font_test.imx > font_test.img

clean:
	rm -f font_test.dis  font_test.bin font_test.elf font_test.imx font_test.img *.o


	
