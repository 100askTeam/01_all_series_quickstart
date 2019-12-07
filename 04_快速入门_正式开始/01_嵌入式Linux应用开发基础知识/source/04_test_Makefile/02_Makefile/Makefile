
test : main.o sub.o
	gcc -o test main.o sub.o
	
main.o : main.c
	gcc -c -o main.o  main.c

sub.o : sub.c
	gcc -c -o sub.o  sub.c	
	
clean:
	rm *.o test -f