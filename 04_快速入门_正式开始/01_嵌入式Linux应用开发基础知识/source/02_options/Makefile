src  := $(shell ls *.c)
objs := $(patsubst %.c,%.o,$(src))

test: $(objs)
	gcc -o $@ $^

%.o:%.c
	gcc -c -o $@ $<

clean:
	rm -f test *.o		