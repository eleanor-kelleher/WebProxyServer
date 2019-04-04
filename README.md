# WebProxyServer
Web Proxy Server created in C for CS3031. Does not actually fetch webpages, but simulates the process of a proxy server. 
Allows the blocking of certain websites from the server terminal.

### How to Run the Program
The server and the client require separate terminals.  The server must be run before the client; if there is no server listening on a port, the client will throw `Error: could not connect.: Connection refused`. After compiling the server, it must be run with the port number as a parameter:
```
./server 4000
```
Once the server code is listening and running, the client can be run with the host and port number as parameters:
```
./client localhost 4000
```
To block a website, you may type, for example, `b www.google.com` then that site will be appended to blockedlist.txt. When the server receives a requested URL sent from the client, it is checked against blocked websites in blockedlist.txt - if a match is found, it sends a message telling the client that access has been denied to that website. 
