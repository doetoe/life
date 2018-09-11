CC = g++ # clang++

all: life

life: life.cpp
	$(CC) --std=c++14 -I. -o life -O3 life.cpp

life.cpp: life.h

lifetime: lifetime.cpp
	$(CC) --std=c++14 -I. -o lifetime -O3 lifetime.cpp

lifetime.cpp: life.h

clean:
	rm -f *.o

realclean: clean
	rm -f life lifetime

