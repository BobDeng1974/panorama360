CC = arm-linux-gnueabihf-g++
API = $(wildcard Api/*.cpp)
BOTTOM=$(wildcard bottom/*.cpp)
CFLAGS = `pkg-config --cflags libopencv libump libmali` 
LIBS =`pkg-config --libs libopencv libump libmali` -lpthread 
C_SRC = $(wildcard *.c)
CPP_SRC = $(wildcard *.cpp)
C_OBJS = $(C_SRC:%.c=%.o)
CPP_OBJS = $(CPP_SRC:%.cpp=%.o) $(API:%.cpp=%.o) $(BOTTOM:%.cpp=%.o) 
H_FILES = $(wildcard *.h) $(wildcard Api/*.h) $(wildcard bottom/*.h)


.PHONY:all clean

all:demo

demo:$(C_OBJS) $(CPP_OBJS) $(H_FILES)
	make -C ./Api
	make -C ./bottom
	$(CC) $(C_OBJS) $(CPP_OBJS) -o $@ $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm *.o demo -f
	make -C ./Api clean
	make -C ./bottom clean

	
