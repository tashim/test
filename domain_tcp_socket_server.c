/* 
lesson 12-2 
file domain_tcp_socket_server, 
this file is part of a program that illustrates the usage of Connection Oriented Domain Sockets

 Description
 -----------
 The program requires 2 files:
 domain_tcp_socket_server (THIS ONE)
 domain_tcp_socket_client 
 
 the Server side creates a Domain socket and waits for the client
 the server part should run before the client
  
   
	
To compile me for Linux, use gcc -ggdb domain_tcp_socket_server.c -o tcp_socket_server 
To execute, type:  ./tcp_socket_server
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

void* thread_client( void * par);
typedef struct {
	int connection;
	int port;
	char ip[20];
}CLIENT;

CLIENT *all[100]={NULL};
pthread_mutex_t mutex;

int main()
{
        int sock,   true = 1;  
        int connected;
 		pthread_t thread1_id;
		CLIENT * pCl;
        struct sockaddr_in server_addr,client_addr;    
        socklen_t sin_size=sizeof(struct sockaddr_in);
        
        
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        {
            perror("Socket");
            exit(1);
        }
	    
	    //set the socket options 
        if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) 
        {
            perror("Setsockopt");
            exit(1);
        }
        
        server_addr.sin_family = AF_INET;         
        server_addr.sin_port = htons(5000);     
        server_addr.sin_addr.s_addr = INADDR_ANY; 
        bzero(&(server_addr.sin_zero),8); 

        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))== -1)
        {
            perror("Unable to bind");
            exit(1);
        }

        if (listen(sock, 1) == -1) 
        {
            perror("Listen");
            exit(1);
        }
		
	    printf("\nTCPServer Waiting for client on port 5000");
        fflush(stdout);


        while(1)
        {  
			connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);
			pCl = (CLIENT*)malloc(sizeof (CLIENT));
			pCl->connection=connected;
			pCl->port=ntohs(client_addr.sin_port);
			strcpy(pCl->ip,inet_ntoa(client_addr.sin_addr));
			printf("\n I got a connection from (%s , %d)",pCl->ip,pCl->port);
			 pthread_mutex_lock(&mutex); 
			 for(int i=0;i<100;i++)
			{
				if(all[i]==NULL)
				{
					all[i]=pCl;
					break;
				}
			}
			 pthread_mutex_unlock(&mutex); 
			pthread_create (&thread1_id, NULL, &thread_client, (void*)pCl);
        }       
	  
      close(sock);
      return 0;
} 



void* thread_client( void * par)
{
	CLIENT * this = (CLIENT*)par;
	int bytes_recieved;
	char recv_data[1024];
	char send_data[1024];
	while (1)
	{
		memset(recv_data,0,sizeof recv_data);
		bytes_recieved = recv(this->connection,recv_data,sizeof(recv_data)-1,0);
		if( bytes_recieved<=0 ) break;
		printf("%d>%s\n",this->port,recv_data);
		sprintf(send_data,"%d>%s\n",this->port,recv_data);
		 pthread_mutex_lock(&mutex); 
		 for(int i=0;i<100;i++)
		{
			if(all[i]!=NULL)
			{
				send(all[i]->connection,send_data,strlen(send_data), 0); 
			}
		}
		 pthread_mutex_unlock(&mutex); 
	}
	printf("%d client disconnect\n",this->port);
	
	 pthread_mutex_lock(&mutex); 
	 for(int i=0;i<100;i++)
	{
		if(all[i]==this)
		{
			all[i]=NULL;
			break;
		}
	}
	 pthread_mutex_unlock(&mutex); 

	close(this->connection);
	free(this);
	return NULL;
}


/*
INADDR_ANY
run without knowing the IP address of the machine it is running on, or, in the case of a machine with multiple network interfaces, 
it allowed your server to receive packets destined to any of the interfaces. 
suppose that a host has interfaces 0, 1 and 2. If a UDP socket on this host is bound using INADDR_ANY and udp port 8000, 
then the socket will receive all packets for port 8000 that arrive on interfaces 0, 1, or 2. 
If a second socket attempts to Bind to port 8000 on interface 1, the Bind will fail since the first socket already ``owns'' that port/interface. 




setsockopt - set the socket options

#include <sys/socket.h>

int setsockopt(int socket, int level, int option_name,
               const void *option_value, socklen_t option_len);

The setsockopt() function shall set the option specified by the option_name argument, 
at the protocol level specified by the level argument, to the value pointed to by the option_value argument for the socket associated with the file descriptor 
specified by the socket argument.
http://pubs.opengroup.org/onlinepubs/009695399/functions/setsockopt.html

SO_REUSEADDR - This basically says that another socket can be opened on the same port on the machine. 
This is useful when recovering from a crash and the socket was not properly closed - 
the app can be restarted and it will simply open another socket on the same port and continue listening.



sin_zero field - used for padding - should be set to zero. (the Linux documentation doesn't even mention it at all),
setting it to zero doesn't seem to be actually  be necessary. 






inet_ntoa() - converts an (Ipv4) Internet network address into an ASCII string in Internet standard dotted-decimal format.

*/
