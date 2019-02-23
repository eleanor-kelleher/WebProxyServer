#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //read & write
#include <string.h>
#include <sys/types.h> //
#include <sys/socket.h> //structs for socket connections
#include <netinet/in.h> //sockaddr_in

//network socket: two-way communication link between client & server
//socket is bound to a port
//server, waits for connection from client by listening to a socket.
//client makes connection request knowing hostname and port being listened to
//server accepts connection & creates a new socket to the same port
//if connection accepted, socket  created on the client side to communicate


void ErrorMessage(const char *msg){
  perror(msg);
  return;
}

int main(int argc, char *argv[]) {
  int socket_fd = 0; //file descriptor
  int new_socket_fd = 0; //file descriptor
  int port = 0;
  int client_addr_length = 0;
  int ret_val = 0; //no of characters string from read() & write()
  char buffer[256]; //read message sent

  //struct that stores IP - IP of server and client stored
  struct sockaddr_in server_addr, client_addr;

  if(argc < 2) {}
    fprintf(stderr, "Missing port\n");
  }

  //AF_INET -> address family of IPv4
  //SOCK_STREAM -> byte stream socket, for TCP
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd < 0) {
    fprintf(stderr, "Error opening socket\n");
  }
 //fills the buffer server_addr with zeros
  bzero((char *)&server_addr, sizeof(server_addr));
  port = atoi(argv[1]);

  server_addr.sin_family = AF_INET;
  //htons does endianness -> converts host byte order to network byte order
  server_addr.sin_port = htons(port);
  //pass IP of the host to s_adder -> INADDR_ANY allows program to work
  //without needing to know the IP address of  machine it is running on
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(socket_fd, (struct sockaddr *)&server_addr,
      sizeof(server_addr(addr)) < 0) {
        perror("Error with connection\n");
        return;
    }

  listen(socket_fd, 5);

}
