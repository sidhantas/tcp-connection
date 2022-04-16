CC=gcc

all: tcp-server.c tcp-client.c 
	$(CC) -g tcp-server.c -o tcp-server -lssl -lcrypto
	$(CC) -g tcp-client.c -o tcp-client -lssl -lcrypto
