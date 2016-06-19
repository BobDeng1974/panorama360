SRC=$(wildcard *.cpp)
CC=arm-linux-gnueabihf-g++
CFLAGS=`pkg-config --cflags libopencv`
all:bottom.o
bottom.o:bottom.h

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@	
clean:
	rm *.o
