PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

dot_line_circle.img : start.S  imx6ull_con.c main.c framebuffer.c geometry.c lcd_manager.c lcd_7_0.c lcd_controller_manager.c
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o lcd_controller_manager.o lcd_controller_manager.c	
	$(CC) -nostdlib -g -c -o lcd_manager.o lcd_manager.c
	$(CC) -nostdlib -g -c -o lcd_7_0.o lcd_7_0.c
	$(CC) -nostdlib -g -c -o imx6ull_con.o imx6ull_con.c	
	$(CC) -nostdlib -g -c -o main.o main.c
	$(CC) -nostdlib -g -c -o framebuffer.o framebuffer.c	
	$(CC) -nostdlib -g -c -o geometry.o geometry.c
	
	$(LD) -T imx6ull.lds -g start.o main.o geometry.o framebuffer.o lcd_controller_manager.o lcd_manager.o lcd_7_0.o imx6ull_con.o -o dot_line_circle.elf
	
	$(OBJCOPY) -O binary -S dot_line_circle.elf  dot_line_circle.bin
	$(OBJDUMP) -D -m arm  dot_line_circle.elf  > dot_line_circle.dis	
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d dot_line_circle.bin dot_line_circle.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin dot_line_circle.imx > dot_line_circle.img

clean:
	rm -f dot_line_circle.dis  dot_line_circle.bin dot_line_circle.elf dot_line_circle.imx dot_line_circle.img *.o


	
