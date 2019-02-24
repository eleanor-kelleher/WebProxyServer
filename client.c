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
    exit(1);
}

void keep_connection(int socket) {
  int ret_val = 0; //no of characters string from read() & write()
  char buffer[256]; //store sent message
  bzero(buffer, 256);

  while(1) {
    printf("Please enter the message:\n");
    //reads stdin and stores 255 bytes of it in buffer
    fgets(buffer, 255, stdin);
    ret_val = write(socket, buffer, strlen(buffer));
    if (ret_val < 0) error("Error: could not write to socket");
    bzero(buffer, 256);
    ret_val = read(socket, buffer, 255);
    if (ret_val < 0) error("Error: could not read from socket");
    printf("%s\n", buffer);
 }
}

int main(int argc, char *argv[]) {
  int socket_fd; //file descriptor
  int port;
  struct sockaddr_in server_addr;
  struct hostent *server; //represents an entry in the hosts database
  char buffer[256];

  if(argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(1);
  }

  port = atoi(argv[2]);
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0)
    error("Error: socket could not be opened.");

  server = gethostbyname(argv[1]); //create hostent with given host name
  if (server == NULL) {
    fprintf(stderr, "Error: no such host exists.\n");
    exit(0);
  }

  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  //copy addr-length of bytes from first arg to the second arg
  bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,
        server->h_length);
  server_addr.sin_port = htons(port);
  if (connect(socket_fd, (struct sockaddr *) &server_addr,
              sizeof(server_addr)) < 0) {
    error("Error: could not connect.");
  }
  keep_connection(socket_fd);
  close(socket_fd);
  return 0;
}
