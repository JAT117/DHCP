/*
John Austin Todd
Compilation: gcc -o c c.c
Execution  : ./c <port_number> [eg. port_number = 5000, where port_number is the UDP server port number]

Discovery
Offer
Request
Ack
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define SERVER "129.120.151.94"		 	//IP address of server cse01.cse.unt.edu
#define BUFLEN 512  				//Max length of buffer
#define TRANS_MIN 100 				//minimum value for transaction id
#define TRANS_MAX 999 				//maximum value for transaction id

void ERROR(char *s)
{
    perror(s);
    exit(1);
}

struct Message
{
	int trans_id;				//transaction ID
	char ip[32];				//IP address
	int lifetime;				//lifetime
};

int main(int argc, char *argv[])
{

	struct sockaddr_in si_other;				//sock address structure
	struct Message *msg = malloc(sizeof(struct Message));	//structure msg of time Message
	int s, i, slen=sizeof(si_other), portno;
	char buf[BUFLEN];
	char message[BUFLEN];
	char ip[32];
	int tid;									//randomly generated transaction ID

	  if (argc != 2)
	  {
			printf("usage: ./c  <port>\n");
			exit(0);
	  }

	system("clear");
	printf("This is a UDP client\n\n");

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)		//checks for socket to send UDP
	{
		ERROR("Error: socket\n");
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	portno = atoi(argv[1]); 					//The port on which to listen for incoming data
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(portno);

	if (inet_aton(SERVER , &si_other.sin_addr) == 0)  // if server does not exist or is not running, terminate program
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	srand(time(NULL));
	tid = rand() % (TRANS_MAX + TRANS_MIN);		//random transaction number assigned
	strcpy(msg->ip, "0.0.0.0");
	msg->trans_id = tid;				//set tid to random transaction ID

	//DHCP Discover
	if (sendto(s, msg, sizeof(*msg) , 0, (struct sockaddr *) &si_other, slen) < 0)
	{
		ERROR("Discover: sendto() Error\n");
	}
	printf("Sent to Server:\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d\n", msg->ip, msg->trans_id, msg->lifetime);

	//DHCP Offer
	if (recvfrom(s, msg, sizeof(*msg), 0, (struct sockaddr *) &si_other, &slen) < 0)
	{
		 ERROR("Offer: recvfrom() Error\n");
	}
	printf("Recieved from Server:\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg->ip, msg->trans_id, msg->lifetime);
	msg->trans_id++;		//increment transaction id by 1
	msg->lifetime = 3600;

	//DHCP Request
	if (sendto(s, msg, sizeof(*msg) , 0, (struct sockaddr *) &si_other, slen) < 0)
	{
		ERROR("Request: sendto() Error\n");
	}

	printf("Sent to Server:\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg->ip, msg->trans_id, msg->lifetime);

	//bzero(buf, BUFLEN);

	//DHCP ACK
	if (recvfrom(s, msg, sizeof(*msg), 0, (struct sockaddr *) &si_other, &slen) < 0)
	{
		 ERROR("ACK: recvfrom() Error\n");
	}

	printf("Recieved from Server:\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg->ip, msg->trans_id, msg->lifetime);

	close(s);
return 0;
}
