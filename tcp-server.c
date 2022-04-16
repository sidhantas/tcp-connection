#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BIND_ADDR "192.168.1.99"

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

int main()
{
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  sockfd = init_tcp_socket();
  int reuse = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
  {
    perror("setsockopt(SO_REUSEADDR) failed");
  }
  printf("TCP socket created\n");
  memset(&serv_addr, 0, sizeof(serv_addr));
  portno = 8080;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(BIND_ADDR);
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("ERROR binding");
    exit(1);
  }

  printf("TCP socket binded to, %s\n", BIND_ADDR);

  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  while (newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen))
  {
    if (newsockfd < 0)
    {
      perror("ERROR on accept");
      exit(1);
    }

    printf("connection accepted\n");

    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);

    if (n < 0)
    {
      perror("ERROR reading from socket");
      exit(1);
    }

    printf("MESSAGE: %s\n", buffer);

    if (strncmp(buffer, "ligths", 6) == 0)
    {
      printf("strings match\n");
      system("sh ~/lights");
    }

    n = write(newsockfd, "I got your message", 18);

    if (n < 0)
    {
      perror("ERROR writing to socket");
      exit(1);
    }
  }

  close(sockfd);
  return 0;
}