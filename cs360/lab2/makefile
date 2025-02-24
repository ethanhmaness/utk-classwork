#CS 360 Lab 2 Makefile

INCLUDES = -I/home/jplank/cs360/include
CFLAGS = -g $(INCLUDES)
LIBDIR = /home/jplank/cs360/objs
LIBS = $(LIBDIR)/libfdr.a
EXECUTABLES = bin/l2p1 bin/l2p2 bin/l2p3

all: $(EXECUTABLES)

bin/l2p1.o: src/l2p1.c
	$(CC) $(CFLAGS) -c src/l2p1.c -o bin/l2p1.o

bin/l2p1: bin/l2p1.o
	$(CC) $(CFLAGS) -o bin/l2p1 bin/l2p1.o $(LIBS)

bin/l2p2.o: src/l2p2.c
	$(CC) $(CFLAGS) -c src/l2p2.c -o bin/l2p2.o

bin/l2p2: bin/l2p2.o
	$(CC) $(CFLAGS) -o bin/l2p2 bin/l2p2.o $(LIBS)

bin/l2p3.o: src/l2p3.c
	$(CC) $(CFLAGS) -c src/l2p3.c -o bin/l2p3.o

bin/l2p3: bin/l2p3.o
	$(CC) $(CFLAGS) -o bin/l2p3 bin/l2p3.o $(LIBS)

#make clean will rid your directory of the executable,
#object files, and any core dumps you've caused
clean:
	$(RM) core $(EXECUTABLES) bin/*.o
