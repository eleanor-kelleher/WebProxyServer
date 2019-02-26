#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //read & write
#include <string.h>
#include <sys/types.h> //
#include <sys/socket.h> //structs for socket connections
#include <netinet/in.h> //sockaddr_in

//network socket: two-way communication link between client & server
//socket is bound to a port
//server, waits for connection from client by listening to a socket
//client makes connection request knowing hostname and port being listened to
//server accepts connection & creates a new socket to the same port
//if connection accepted, socket created on the client side to communicate
FILE *fp_blocked;

void error(const char *message){
  perror(message);
  exit(1);
}

void block_sites() {
  char buffer[256];
  bzero(buffer, 256);
  printf("Do you wish to block any URLs? (y/n)\n");
  fgets(buffer, 256, stdin);

  if(buffer[0] == 'y' || buffer[0] == 'Y') {
    printf("Type 'b' followed by a URL to block it.\nType anything else to exit.\n");
    int loop = 1;

    while(loop) {
      fgets(buffer, 255, stdin);

      if(buffer[0] == 'b' && buffer[1] == ' ') {
        memmove(buffer, buffer + 2, strlen(buffer));
        fp_blocked = fopen("blockedlist.txt", "a");
        fprintf(fp_blocked, "%s", buffer);
        fclose(fp_blocked);
        printf("Added this site to the blocklist: %s", buffer);
      }
      //else if(buffer[0] == 'q'){
      //  loop = 0;
      //}
      else {
        printf("Do you wish to quit? (y/n)\n");
        fgets(buffer, 255, stdin);

        if(buffer[0] == 'y' || buffer[0] == 'Y') {
          loop = 0;
        }
        else if(buffer[0] == 'n' || buffer[0] == 'N') {}
        else {
          printf("Please enter a valid answer.\n");
        }
      }
    }
  }
  else if(buffer[0] == 'n' || buffer[0] == 'N') {}
  else {
    printf("Please enter a valid answer.\n");
  }
}

int check_blocked_site(char *buffer) {
  int site_count = 0;
  char ch;
  FILE *fp_read_blocked = fopen("blockedlist.txt", "r");
  FILE *fp_read_blocked2 = fopen("blockedlist.txt", "r");
  //get number of blocked sites in the list
  while(!feof(fp_read_blocked)) {
    ch = fgetc(fp_read_blocked);
    if(ch == '\n') {
      site_count++;
    }
  }
  char sites[site_count][2048];
  for(int i = 0; i < site_count; i++) {
    fscanf(fp_read_blocked2, "%s", sites[i]);
    char *blocked_site = strtok(buffer, "\n");
    //printf("%s, %s\n", sites[i], blocked_site);
    if(strcmp(blocked_site, sites[i]) == 0) {
      return 1; //if same website is found in blocklist, do not return webpage.
    }
  }
  fclose(fp_read_blocked);
  return 0;
}

void keep_connection(int socket) {
  while(1) {
    int ret_val = 0; //no of characters string from read() & write()
    char buffer[256]; //store sent message
    bzero(buffer, 256);
    ret_val = read(socket, buffer, 255);

    if(ret_val < 0) error("Error: could not read socket.");
    printf("Client requested URL: %s\n", buffer);
    if(check_blocked_site(buffer) == 1) {
      printf("Client has attempted to access a blocked website: %s\n", buffer);
      ret_val = write(socket, "ACCESS DENIED: this site is blocked.", 36);
      if(ret_val < 0) error("Error: could not write to socket.");
    }
    else {
      ret_val = write(socket, "Webpage sent from server.", 25);
      if(ret_val < 0) error("Error: could not write to socket.");
      printf("Webpage sent to client.\n");
    }
  }
}



int main(int argc, char *argv[]) {
  int socket_fd = 0; //file descriptor
  int new_socket_fd = 0; //file descriptor
  int port = 0;
  int process_id = 0; //each client socket connection gets a new process
  int client_addr_length = 0;
  //struct that stores IP - IP of server and client stored
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;

  if(argc < 2) {
    fprintf(stderr, "Error: please enter a port number.\n");
    exit(1);
  }

  //AF_INET -> address family of IPv4
  //SOCK_STREAM -> byte stream socket, for TCP
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  printf("Server socket created.\n");
  if(socket_fd < 0) {
    fprintf(stderr, "Error: socket could not be opened.\n");
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
  printf("Connecting server...\n");
  if(bind(socket_fd, (struct sockaddr *) &server_addr,
          sizeof(server_addr)) < 0) {
    error("Error: connection not bound.\n");
  }
  printf("Server connected.\n");
  listen(socket_fd, 5); //allow up to 5 connections
  block_sites();
  printf("Server listening on port %d...\n", port);
  //1 give size to client_addr_length to pass on to accept()
  //2 create another socket with socket descriptor, client address & its size
  //3 if fails return value < zero and will produce an error
  //accept() will block  process until a client connects
  client_addr_length = sizeof(client_addr);
  //loop will allow multiple socket connections

  while(1) {
    new_socket_fd = accept(socket_fd, (struct sockaddr *)&client_addr,
                           &client_addr_length);
    if(new_socket_fd < 0) {
      error("Error: server could not accept client.");
    }
    process_id = fork();
    if(process_id < 0) {
      error("Error: fork unsuccessful.");
    }
    if(process_id == 0) {
      close(socket_fd);
      keep_connection(new_socket_fd);
      exit(0);
    }
    else close(new_socket_fd);
  }

  close(socket_fd);
  return 0; //never reached
}
