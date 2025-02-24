EXECUTABLES = bin/gold

CFLAGS ?= -Wall -Wextra -std=c++98

all: $(EXECUTABLES)

clean:
	rm -f a.out bin/*

bin/gold: src/gold.cpp
	g++ $(CFLAGS) -o bin/gold src/gold.cpp

