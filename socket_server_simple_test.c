// Server side C/C++ program to demonstrate Socket programming 
// Programming in stdC99
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
//the prot we hope to attach
#define PORT 12000 
#define USERS 5
/*---------------------------------------------
 *    create socket 
 *			||
 *		set socket  port
 *			||
 *			bind			      client -->send request
 *			||						     ||
 *		Listen  <------------------     Connect
 *			||								||		
 *		Accecpt -------> send/receive <-------sending/recieving 
 *
 *	reference  :
 *	socket funtion :
 *	https://www.systutorials.com/docs/linux/man/2-setsockopt/
 *	https://www.systutorials.com/docs/linux/man/2-bind/
 *	socket connection:	
 *	https://www.geeksforgeeks.org/socket-programming-cc/
 *	socket struct addr_in
 *	https://baike.baidu.com/item/sockaddr_in/3917215
 *
*/





int main(int argc, char const *argv[]) 
{ 	

	//server_fd  it's same as a file handle to mark the socket
	int server_fd, new_socket, valread; 
	//defind in netinet/in.h
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
	char *hello = "Hello from server"; 
	
	// Creating socket file descriptor 
	//AF_INET <=>ipv4   AF_INET6<=>ipv6
	//SOCK_STREAM <=> TCP protocol  SOCK_DGRAM <=>UDP protocol
	
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port  
	//if erro return -1
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	//htons() to change the format of common number into net trans-format
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port  
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	//USERS definded by user
	if (listen(server_fd,USERS ) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	//when listen the connection of a client , it will create a new socket 
	//to start the send/recieve work.
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 
	//after the connection is build it will start here
	//once a client is connected the program will ret 0 and exit if not using while
	//However by using while(1) is not recommended for huge amount of client when in realese version.
	//So How to figure out this problem?
	//i'm not sure why while(1) is not good ,but for multiple thread is not a good way 
	// i think by using memory pool is a good idea ,To avoid the memory recollect and assgin.
	//And we can build the fork in one Thread. Or create a thread only in I/O opreation.
	//but for demo we use while(1) first ,I will try to prove this soon.
	int chos;
	while(chos!=-1)
	{		
		valread = read( new_socket , buffer, 1024); 
		printf("%s\n",buffer ); 
		send(new_socket , hello , strlen(hello) , 0 ); 
		printf("Hello message sent\n");	
	}
	return 0;
	
	 
	return 0; 
} 

