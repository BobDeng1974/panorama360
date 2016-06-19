CC = arm-linux-gnueabihf-g++
API = $(wildcard Api/*.cpp)
BOTTOM =$(wildcard bottom/*.cpp)
CFLAGS = `pkg-config --cflags libopencv`
LIBS = `pkg-config --libs libopencv` -pthread
C_SRC = $(wildcard *.c)
CPP_SRC = $(wildcard *.cpp)
C_OBJS = $(C_SRC:%.c=%.o)
CPP_OBJS = $(CPP_SRC:%.cpp=%.o) $(API:%.cpp=%.o) $(BOTTOM:%.cpp=%.o)
H_FILES = $(wildcard *.h) $(wildcard Api/*.h) $(wildcard bottom/*.h)

.PHONY: all clean

all:getMap

getMap:$(C_OBJS) $(CPP_OBJS) $(H_FILES)
	make -C ./Api
	make -C ./bottom
	$(CC) $(C_OBJS) $(CPP_OBJS) -o getMap $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o getMap -f
	make -C ./Api clean
	make -C ./bottom clean


