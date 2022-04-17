CC=gcc

all: tcp-server tcp-client

tcp-client: tcp-client.c 
	$(CC) -g tcp-client.c -o tcp-client -lssl -lcrypto

tcp-server: tcp-server.c
	$(CC) -g tcp-server.c -o tcp-server -lssl -lcrypto
