#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

int init_tcp_socket()
{
  /*
   creates a tcp socket and returns its fd, exits program
   if no file descriptor is made
   */
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
  {
    perror("Could not create socket");
    exit(1);
  }
  return sockfd;
}

#define BIND_ADDR "192.168.1.99"
#define BIND_PORT "8080"

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  char buffer[256];

  if (argc < 2)
  {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  portno = atoi(argv[1]);

  sockfd = init_tcp_socket();
  struct addrinfo hints, *server, *p;

  struct sockaddr host_addr;
  host_addr.sa_family = AF_INET;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_addr = &host_addr;

  if (getaddrinfo(BIND_ADDR, BIND_PORT, &hints, &server) != 0)
  {
    fprintf(stderr, "ERROR, no such host\n");
    exit(1);
  }
  /* Now connect to the server */

  if (connect(sockfd, server->ai_addr, server->ai_addrlen) < 0)
  {
    perror("ERROR connecting");
    exit(1);
  }
  /* Now ask fo r a message from the user, this message
   * will be read by server
   */

  printf("Please enter the message: ");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);

  /* Send message to the server */
  n = write(sockfd, buffer, strlen(buffer));

  if (n < 0)
  {
    perror("ERROR writing to socket");
    exit(1);
  }

  /* Now read server response */
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);

  if (n < 0)
  {
    perror("ERROR reading from socket");
    exit(1);
  }

  printf("%s\n", buffer);
  close(sockfd);
  return 0;
}