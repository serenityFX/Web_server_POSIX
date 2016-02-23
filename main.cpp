#include <sys/ioctl.h>
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<string>
 
#include<pthread.h> //for threading , link with lpthread
 
void process(int d);
 
int main(int argc , char *argv[])
{
	std::string		SrvAddress;
	std::string		RootDir;
	std::uint16_t	SrvPort;
	int opt = 0;

	while ((opt = getopt(argc, argv, "h:p:d:")) != -1) {
		switch (opt) {
		case 'h':
			SrvAddress.assign(optarg);
			break;
		case 'p':
			SrvPort = atoi(optarg);
			break;
		case 'd':
			RootDir.assign(optarg);
			break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s -h <ip> -p <port> -d <directory>\n",
				argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char *message;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(SrvAddress);
    server.sin_port = htons( SrvPort );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , SOMAXCONN);
     
	int pid;
    while(true)
    {
		new_socket = accept(socket_desc, 0, 0);
		
		puts("Connection accepted");
		
		pid = fork();
		
		if (pid == 0 )
		{
			close( socket_desc );
			process(new_socket);
			shutdown(new_socket,SHUT_RDWR);
			close(new_socket);
			return 0;
		}
		else
			close(new_socket);       
    }
     
    return 0;
}

void process(int d)
{
	int len = 0;
	char Buffer[4096];
while(1)
{
	ioctl(d, FIONREAD, &len);
	
	if (len > 0)
{ 
		len = recv(d,Buffer,len,MSG_NOSIGNAL);
		break;
}
}	


	printf("READ FROM SOCKET: %s\n", Buffer);
}
 



