OBJS=binaryStory.c
CFLAGS= -Wall -o

all:
	$(CC) $(CFLAGS) story $(OBJS)

clean:
	-rm -f *.o storys
