
test : main.o sub.o
	gcc -o test main.o sub.o
	
%.o : %.c
	gcc -c -o $@  $<
	
clean:
	rm *.o test -f
	