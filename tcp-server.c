#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BIND_ADDR "192.168.1.99"

int init_tcp_socket() {
    /*
     creates a tcp socket and returns its fd, exits program
     if no file descriptor is made
     */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("Could not create socket");
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }
    return sockfd;
}

void configure_server_address(struct sockaddr_in *serv_addr, char *ip_addr, int port) {
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr->sin_port = htons(port);
}

void bind_and_listen(struct sockaddr_in *serv_addr, int sockfd) {
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR binding");
        exit(1);
    }
    listen(sockfd, 5);
    printf("TCP socket binded to, %s\n", BIND_ADDR);
    return;
}

void handle_socket_message(char *buffer, int n) {
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    if (strncmp(buffer, "lights\n", 7) == 0) {
        system("sh ~/lights");
    } else if (strncmp(buffer, "fan\n", 4) == 0) {
        system("sh ~/fan");
    }

    return;
}

int main() {
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = init_tcp_socket();

    printf("TCP socket created\n");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = 8080;

    configure_server_address(&serv_addr, BIND_ADDR, portno);
    bind_and_listen(&serv_addr, sockfd);
    clilen = sizeof(cli_addr);

    while (newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen)) {
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        printf("connection accepted\n");

        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        handle_socket_message(buffer, n);
        n = write(newsockfd, "I got your message", 18);
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

    close(sockfd);
    return 0;
}
