CC=gcc

all: tcp-server.c tcp-client.c 
	$(CC) -g tcp-server.c -o tcp-server
	$(CC) -g tcp-client.c -o tcp-client
