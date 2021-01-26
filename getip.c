/*
 * getip.c
 * 
 * Copyright 2021 tashim <tashim@ubuntu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 //-------------Include Section --------------------
#include<stdio.h>	//printf
#include<string.h>	//memset
#include<errno.h>	//errno
#include<sys/socket.h>
#include<netdb.h>
#include<ifaddrs.h>
#include<stdlib.h>
#include<unistd.h>

#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>


 #define ETH "ens33"


void set_ip();
int check_connetcion();
int get_ip(char *buff);

//==============================
/*
 *  Function: check_connetcion
 *  Description: check if the device is connected to the Ethernet
 *  Parameters: None
 *  Return: 1 ==> connected to the Ethernet, 0 ==> disconnected
 */
int check_connetcion()
{
	FILE * f;
	char con;
	char name[80];
	sprintf(name,"/sys/class/net/%s/carrier",ETH);
	f = fopen(name,"r");
	if (!f)
	{
		return 0;
	}
	fscanf(f,"%c",&con);
	fclose(f);

	return (con-'0');
}

//=================
/*
 *  Function: get_ip
 *  Description: get ip address of the socket
 *  			 and check if the device connected to the Internet
 *  Parameters: buff ==> this is an output, it will be filled by the
 *  					 IP address
 *  Return: if the device connected to the Internet or not
 */
int get_ip(char *buff)
{
	int fm = AF_INET;
    struct ifaddrs *ifaddr, *ifa;
	int family , s;
	char host[NI_MAXHOST];
	*buff = 0;
	char*head = buff;

	if ( !check_connetcion(ETH) )
	{
		return 0;
	}

	if (getifaddrs(&ifaddr) == -1)
	{
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	//Walk through linked list, maintaining head pointer so we can free list later
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
		{
			continue;
		}

		family = ifa->ifa_addr->sa_family;

		if(strcmp( ifa->ifa_name , ETH) == 0)
		{
			if (family == fm)
			{
				s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);

				if (s != 0)
				{
					printf("getnameinfo() failed: %s\n", gai_strerror(s));
					return(EXIT_FAILURE);
				}
				int ii;
				sscanf(host,"%d",&ii);
				printf(" ip = %s<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n",host);
				if(ii==169)break;
				sprintf(buff,"%s",host);
			}
		}
	}

	if(buff != head)
		*(buff-1)  = 0 ;

	if(strlen(buff)<6)
	{
		set_ip();
	}

	freeifaddrs(ifaddr);

	return 1;
}

//==========================
/*
 *  Function: set_ip
 *  Description: Set the device IP address
 *  Parameters: None
 *  Return: None
 */
void set_ip()
{
		char command[200];
		sprintf(command,"ifconfig "ETH" %s netmask 255.255.255.0 up","192.168.229.123");
		system(command);
//struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
//inet_pton(AF_INET, "10.12.0.1", &addr->sin_addr);

}


#include <stdio.h>

 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
 
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
 
int main(int argc, char **argv)
{

     char ip_address[15];
    int fd;
    struct ifreq ifr;
    struct sockaddr_in *addr;
     
    /*Read IP Address*/
    printf("Enter Ip Address: ");
    scanf("%s",ip_address);
     
    /*AF_INET - to define network interface IPv4*/
    /*Creating soket for it.*/
    fd = socket(AF_INET, SOCK_DGRAM, 0);
     
    /*AF_INET - to define IPv4 Address type.*/
    //ifr.ifr_addr.sa_family = AF_INET;
     
    /*eth0 - define the ifr_name - port name
    where network attached.*/
    memcpy(ifr.ifr_name, "ens33", IFNAMSIZ-1);
     
    /*defining the sockaddr_in*/
 //   addr=(struct sockaddr_in *)&ifr.ifr_addr;
     
    /*convert ip address in correct format to write*/
  //  inet_pton(AF_INET,ip_address,&addr->sin_addr);
     
    /*Setting the Ip Address using ioctl*/
   // ioctl(fd, SIOCSIFADDR, &ifr);
    /*closing fd*/
     
   // printf("IP Address updated sucessfully.\n");
     
    /*Getting the Ip Address after Updating.*/
     
    /*clear ip_address buffer with 0x20- space*/
    memset((unsigned char*)ip_address,0x20,15);
    ioctl(fd, SIOCGIFADDR, &ifr);
    /*Extracting Ip Address*/
    strcpy(ip_address,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
     
    printf("Updated IP Address is: %s\n",ip_address);
     
   close(fd);
     return 0;
}

