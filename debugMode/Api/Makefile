SRC=$(wildcard *.cpp)
CFLAGS=`pkg-config --cflags libopencv`
CC=arm-linux-gnueabihf-g++
all:Api.o
Api.o:Api.h

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	rm *.o 

