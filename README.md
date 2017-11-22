# DHCP
DHCP server/server in c

Server:
Compilation: gcc -o s s.c
Execution  : ./s <port_number> [eg. port_number = 5000, where port_number is the UDP server port number]

Client:
Compilation: gcc -o c c.c
Execution  : ./c <port_number> [eg. port_number = 5000, where port_number is the UDP server port number]

Discover
Offer
Request 
ACK

Server reads off from IPaddress.txt and assignes an IP to a single client at at time, removing fromthe list
