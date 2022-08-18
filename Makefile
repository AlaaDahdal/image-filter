
CC=gcc

default: imfilter

imfilter: imfilter.c
	$(CC) imfilter.c  -pthread -o imfilter 

clean:
	rm -f imfilter
