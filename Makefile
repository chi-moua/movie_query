# define the commands we'll use for compilation and library building
AR = ar
ARFLAGS = rcs
CC = gcc

GOOGLE_TEST_INCLUDE=${HOME}/src/googletest-release-1.8.0/googletest/include/

# define useful flags to cc/ld/etc.
CFLAGS = -g -Wall -I. -I.. -Ihtll -pthread -no-pie
LDFLAGS = -L.


#define common dependencies
OBJS = libs/docset.o libs/docidmap.o libs/fileparser.o libs/filecrawler.o \
	libs/movieIndex.o libs/LinkedList.o libs/Hashtable.o libs/movie.o \
	libs/queryprocessor.o libs/queryprotocol.o

HEADERS = fileparser.h filecrawler.h docidmap.h movieIndex.h docset.h \
	movie.h Hashtable.h LinkedList.h queryprotocol.h


# compile everything
all: program

program: server multiserver client 

server: queryserver.c
	gcc $(CFLAGS) -g -I includes -o queryserver queryserver.c $(OBJS) -L.

multiserver: multiserver.c
	gcc $(CFLAGS) -g -I includes -o multiserver multiserver.c $(OBJS) -L.

runserver:
	./queryserver data_small/ 1500

runmultiserver:
	./multiserver data_small/ 1500

client: queryclient.c
	gcc $(CFLAGS) -g -I includes -o queryclient queryclient.c $(OBJS) -L.

runclient:
	./queryclient 127.0.0.1 1500

clean: FORCE
	/bin/rm -f *.o *~ main queryserver multiserver queryclient

FORCE:
