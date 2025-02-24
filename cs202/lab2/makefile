EXECUTABLES = bin/bigwhite \
              bin/pgminfo \
              bin/neg \
              bin/hflip \
              bin/vflip \

CFLAGS ?= -Wall -Wextra -std=c++98

all: $(EXECUTABLES)

clean:
	rm -f a.out bin/*

bin/bigwhite: src/bigwhite.cpp
	g++ $(CFLAGS) -o bin/bigwhite src/bigwhite.cpp

bin/pgminfo: src/pgminfo.cpp
	g++ $(CFLAGS) -o bin/pgminfo src/pgminfo.cpp

bin/neg: src/neg.cpp
	g++ $(CFLAGS) -o bin/neg src/neg.cpp

bin/hflip: src/hflip.cpp
	g++ $(CFLAGS) -o bin/hflip src/hflip.cpp

bin/vflip: src/vflip.cpp
	g++ $(CFLAGS) -o bin/vflip src/vflip.cpp
