#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //read & write
#include <string.h>
#include <sys/types.h> //
#include <sys/socket.h> //structs for socket connections
#include <netinet/in.h> //sockaddr_in
#include <netdb.h> //hostent

void error(const char *message) {
    perror(message);
    exit(0);
}

int main(int argc, char *argv[]) {
  int socket_fd;
  int port;
  int ret_val;
  struct sockaddr_in server_addr;
  struct hostent *server; //represents an entry in the hosts database
  char buffer[256];

  if(argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  port = atoi(argv[2]);
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0)
    error("Error: socket could not be opened.");

  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr, "Error: no such host.\n");
    exit(0);
  }

  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,
        server->h_length);
  server_addr.sin_port = htons(port);
  if (connect(socket_fd, (struct sockaddr *) &server_addr,
              sizeof(server_addr)) < 0)
    error("Error: could not connect.");
  printf("Please enter the message: ");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);
  ret_val = write(socket_fd, buffer, strlen(buffer));
  if (ret_val < 0)
    error("Error: could not write to socket");
  bzero(buffer, 256);
  ret_val = read(socket_fd, buffer, 255);
  if (ret_val < 0)
    error("Error: could not read from socket");
  printf("%s\n", buffer);
  close(socket_fd);
  return 0;
}
