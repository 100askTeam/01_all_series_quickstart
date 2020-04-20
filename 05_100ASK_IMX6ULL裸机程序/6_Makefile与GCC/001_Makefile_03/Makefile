VERSION = 1.0.0
SOURCE = $(wildcard ./src/*.c)
OBJECT = $(patsubst %.c, %.o, $(SOURCE))
	
INCLUEDS = -I ./inc
	
TARGET  = 100ask
CC      = gcc
CFLAGS  = -Wall -g
	
$(TARGET): $(OBJECT)
	@mkdir -p output/
	$(CC) $^ $(CFLAGES) -o output/$(TARGET)_$(VERSION)

%.o: %.c
	$(CC) $(INCLUEDS) $(CFLAGES) -c $< -o $@

.PHONY:clean
clean:
	@rm -rf $(OBJECT) output/
