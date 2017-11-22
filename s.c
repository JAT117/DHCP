/*
John Todd
Compilation: gcc -o s s.c
Execution  : ./s <port_number> [eg. port_number = 5000, where port_number is the UDP server port number]

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
#include <error.h>
#define BUFLEN 512  	//buffer
#define IP_COUNT 10	//Number of IP addresses in the file

typedef enum {TRUE = 1, FALSE = 0} bool;  //beginning of time stuff

struct Message{			//message struct
	int trans_id;  //transaction ID
	char ip[32];		//IP
	int lifetime;		//Lifetime
};

struct address{				  //Address structure
	char ip[32];
	bool available;
};

void ERROR(char *s){			//error
    perror(s);
    exit(1);
}

//returns index of available IP address
int avail_ip(struct address arr[]){   //declares address struict called arr
	int i;
	for(i = 0; i < IP_COUNT; i++)
	{
		if(arr[i].available == TRUE)
		return i;
	}
return -1;
}

int main(int argc, char *argv[])
{
	FILE *fp;				              //file pointer to read in the IP addresses file
	char line[256];				              //line for each line from the file
	struct address addr_arr[IP_COUNT];		      //holds all the ip addresses from the file
	int i, index, len;		    	      //iterator, index, and length
	char* token;		        		      //to use string token to get rid of new line character in the ip addresses file

	fp = fopen("IPaddress.txt", "rw");		      //IP address file I/O to read and write
	if(fp == NULL)
	  {
		ERROR("ERROR opening file\n");
 	   }

	fgets(line, sizeof(line), fp);			      //get the first line from the file
	i = 0;					              //reset set iterator equal to 0

	do{						      //get the string without the endline character
		token = strtok(line, "\n");
		strcpy(addr_arr[i].ip, token);
		addr_arr[i].available = TRUE;
		i++;

	}while(fgets(line, sizeof(line), fp));	//while there are still lines to be read from the file

  	struct sockaddr_in si_me, si_other;
	int s, slen = sizeof(si_other), recv_len, portno;
	char buf[BUFLEN], message[1024];

	//	for(i = 0; i < IP_COUNT; i++)		//print all addresses in list
	//	{
	//		printf("%s\n", addr_arr[i].ip);
	//	}

	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		ERROR("socket\n");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));
	portno = atoi(argv[1]);             //The port on which to listen for incoming data

	si_me.sin_family = AF_INET;		    	//various socket programming necessities
	si_me.sin_port = htons(portno);			//network  endiness
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port

	if(bind(s, (struct sockaddr*) &si_me, sizeof(si_me)) < 0)
	{
		ERROR("bind\n");
	}

	system("clear");  //clears the screen of junk

	printf("This is a DHCP UDP server\n\n");
	struct Message *msg = malloc(sizeof(struct Message));  //create a structure msg with template Message

	while(1)    	//keep listening for data
	{
		printf("Waiting for client's message...\n\n");
		fflush(stdout);

		//DHCP Discover
		if ((recv_len = recvfrom(s, msg, sizeof(*msg), 0, (struct sockaddr *) &si_other, &slen)) < 0)
		{
			ERROR("Discovery: recvfrom() Error\n");
		}
		printf("Recieved from Client:\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg->ip, msg->trans_id, msg->lifetime);
		if(strcmp(msg->ip, "0.0.0.0") == 0)
 		   {
	    		index = avail_ip(addr_arr);
 		   }
		strcpy(msg->ip, addr_arr[index].ip);
		msg->lifetime, 3600;

		//DHCP Offer
		if (sendto(s, msg, sizeof(msg), 0, (struct sockaddr *) &si_other, slen) < 0)
		{
			ERROR("Offer: sendto() Error\n");
		}

                strcpy(msg->ip, token);

		printf("Sent to Client:\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg->ip, msg->trans_id, msg->lifetime);

		//DHCP Request
		if ((recv_len = recvfrom(s, msg, sizeof(*msg), 0, (struct sockaddr *) &si_other, &slen)) < 0)
		{
			ERROR("Request: recvfrom() Error\n");
		}

		msg->lifetime = 3600;

		printf("Recieved from Client:\n\ttiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg->ip, msg->trans_id, msg->lifetime);
		if(strcmp(msg->ip, addr_arr[index].ip) == 0)
	        {
			addr_arr[index].available = FALSE;
                }
		strcpy(msg->ip, addr_arr[index].ip);

		msg->lifetime = 3600;

		//DHCP ACK
		if (sendto(s, msg, sizeof(msg), 0, (struct sockaddr *) &si_other, slen) < 0)
		{
			ERROR("ACk: sendto() Error\n");
		}
		printf("Sent to Client:\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg->ip, msg->trans_id, msg->lifetime);
	}

	fclose(fp);
	close(s);

return 0;
}
