#include <sys/ioctl.h>
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<string>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
 
#include<pthread.h> //for threading , link with lpthread
 
void process(int d, std::string &rootDir);
void skeleton_daemon();

static const char not_found[] 	= "HTTP/1.0 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n";
static const char templ[] 		= "HTTP/1.0 200 OK\r\nContent-length: %d\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n%s";
static const char empty[] 		= "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";

int main(int argc , char *argv[])
{
	std::string		SrvAddress;
	std::string		RootDir;
	std::uint16_t	SrvPort;
	int opt = 0;
	
	if(argc < 6)
{
			fprintf(stderr, "Usage: %s -h <ip> -p <port> -d <directory>\n",
				argv[0]);
			return 0;
}

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
	
	skeleton_daemon();
	
	
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
    server.sin_addr.s_addr = inet_addr(SrvAddress.c_str());
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
			process(new_socket,RootDir);
			shutdown(new_socket,SHUT_RDWR);
			close(new_socket);
			return 0;
		}
		else
			close(new_socket);       
    }
     
    return 0;
}

void process(int d,std::string &RootDir)
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
	
	char req[3];
	memcpy(req,Buffer,3);
	
	//std::string str(Buffer);
	
	if( strcmp(req,"GET") == 0)
	{
		char buff[2048] = {0};
		char file[1024] = {0};
		char fileData[1024] = {0};
		
		char *ptr = &Buffer[4];
		char *ptrF = file;
		
		while(ptr && (*ptr != ' ' && *ptr != '?'))
		{
			*ptrF = *ptr;
			ptr++;
			ptrF++;
		}
		
		if(strcmp(file,"HTTP/1.0") == 0)
		{
			
			send(d, empty, sizeof(empty), 0);
		}
		else if (strcmp(file,"/") == 0)
		{
			printf("EMPTY request");
			send(d, not_found, sizeof(not_found), 0);
		}
		else
		{
			RootDir.append(file);
		
			int fd = open (RootDir.c_str(), O_RDONLY);
		
			if(fd != -1)
			{
				int size = 0;
				char ch;
				char *ptrData = fileData;
				while ((read (fd, &ch, 1)) > 0)
				{
					*ptrData = ch;
					ptrData++;
					size++;
				}
				sprintf(buff,templ,size,fileData);
				send(d, buff, sizeof(buff), 0);
			}
			else
			send(d, not_found, sizeof(not_found), 0);
		}	
	}
	else
		send(d, not_found, sizeof(not_found), 0);
	
	
}

void skeleton_daemon()
{
	 int pid = fork();

    if (pid == -1) // если не удалось запустить потомка
    {
        // выведем на экран ошибку и её описание
        printf("Error: Start Daemon failed (%s)\n", strerror(errno));
        
        exit(EXIT_FAILURE);
    }
    else if (!pid) // если это потомок
    {
        // данный код уже выполняется в процессе потомка
        // разрешаем выставлять все биты прав на создаваемые файлы, 
        // иначе у нас могут быть проблемы с правами доступа
        umask(0);
        
        // создаём новый сеанс, чтобы не зависеть от родителя
        setsid();
        
        // переходим в корень диска, если мы этого не сделаем, то могут быть проблемы.
        // к примеру с размантированием дисков
        chdir("/");
        
        // закрываем дискрипторы ввода/вывода/ошибок, так как нам они больше не понадобятся
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        
    }
    else // если это родитель
    {
        // завершим процес, т.к. основную свою задачу (запуск демона) мы выполнили
       exit(EXIT_SUCCESS);
    }
}
 



