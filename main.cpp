#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
 
#include<pthread.h> //for threading , link with lpthread
 
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
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
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , SOMAXCONN);
     
    while(true)
    {
		if(new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c) != -1)
		{
			puts("Connection accepted");
         
        //Reply to the client
        message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
        write(new_socket , message , strlen(message));
         
        pthread_t sniffer_thread;
        new_sock = static_cast<int *>(malloc(1));
        *new_sock = new_socket;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        pthread_detach(sniffer_thread);
        puts("Handler assigned");
		}
        
    }
     
    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[4096] = {0};
	
	printf("I'm HERE!!!!!\n");
     
	 unsigned int counter = 0;
	 while(true)
	 {
		 if(read_size = recv(sock , client_message, 4096 , MSG_NOSIGNAL) > 0)
		 {
			 counter += read_size;
			 break;
		 }
	 }
     
    //Receive a message from client
	
    while( (read_size = recv(sock , client_message + counter , 4096 - counter , MSG_NOSIGNAL)) > 0 )
    {
		counter += read_size;
        //Send the message back to client
    }
     
    //Free the socket pointer
	shutdown(sock,SHUT_RDWR);
	close(sock);
    free(socket_desc);
     
	 printf("receive from client %s\n",client_message);
	 
    return 0;
}


do 
       {
           n = recv(newsockfd, buffer, sizeof(buffer), MSG_DONTWAIT);  // Use recv instead of read
           if (n>0) 
           {
              buffer[n] = 0;
              msg += buffer;                    // Actually increase the "large" buffer
              pkt++;
           }
       } while (n > 0 || ((n == -1) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)) && pkt == 0));

