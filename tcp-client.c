#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BIND_ADDR "192.168.1.99"
#define BIND_PORT "8080"

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
  return sockfd;
}

void init_addr_info(struct sockaddr *host_addr, struct addrinfo **server) {
  struct addrinfo hints;
  bzero(&hints, sizeof(hints));

  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_addr     = host_addr;

  if (getaddrinfo(BIND_ADDR, BIND_PORT, &hints, server) != 0) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(1);
  }
}

SSL_CTX *create_client_context() {
  const SSL_METHOD *method = TLS_client_method();
  SSL_CTX          *ctx    = SSL_CTX_new(method);
  if (!ctx) {
    perror("Could not create SSL context");
    exit(1);
  }

  return ctx;
}

void configure_client_context(SSL_CTX *ctx) {
  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
  if (!SSL_CTX_load_verify_locations(ctx, "cert.pem", NULL)) {
    perror("Could not configure client context");
    exit(1);
  }
  return;
}

int main(int argc, char *argv[]) {
  int              sockfd, portno, n;
  char             buffer[256];
  struct addrinfo *server, *p;
  struct sockaddr  host_addr;
  SSL_CTX         *ssl_ctx;
  SSL             *ssl;

  ssl_ctx = create_client_context();
  configure_client_context(ssl_ctx);
  sockfd = init_tcp_socket();

  host_addr.sa_family = AF_INET;

  init_addr_info(&host_addr, &server);

  if (connect(sockfd, server->ai_addr, server->ai_addrlen) < 0) {
    perror("ERROR connecting");
    exit(1);
  }
  printf("TCP Connection Successful\n");

  ssl = SSL_new(ssl_ctx);
  SSL_set_fd(ssl, sockfd);
  SSL_set_tlsext_host_name(ssl, BIND_ADDR);
  SSL_set1_host(ssl, BIND_ADDR);

  if (SSL_connect(ssl) == 1) {
    printf("SSL connection successful");
    printf("Please enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    n = SSL_write(ssl, buffer, strlen(buffer));
    if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
    }
    bzero(buffer, 256);
    n = SSL_read(ssl, buffer, 255);
    if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
    }
  } else {
    printf("SSL connection to server failed\n\n");

    ERR_print_errors_fp(stderr);
  }

  /* Send message to the server */

  /* Now read server response */

  printf("%s\n", buffer);
  close(sockfd);
  return 0;
}