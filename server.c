#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

//thread function
void *connection_handler(void *);

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;

	//	Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		perror("Could not create socket\n");
		return -1;
	}
	printf("Socket created\n");

	//	Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//	Binding
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Binding error\n");
		return -1;
	}
	printf("Bind successfully done\n");

	//	Listen
	listen(socket_desc , 3);

	//	Accept and incoming connection
	printf("Waiting for incoming connections...\n");
	c = sizeof(struct sockaddr_in);

	//	Accept and incoming connection
	printf("Waiting for incoming connections...\n");
	c = sizeof(struct sockaddr_in);
	
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		printf("Connection accepted\n");

		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;

		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("Could not create thread\n");
			return -1;
		}

		//	Now join the thread , so that we dont terminate before the thread
		//	pthread_join( sniffer_thread , NULL);
		printf("Handler assigned\n");
	}

	if (client_sock < 0)
	{
		perror("Accepting error");
		return -1;
	}

	return 0;
}

/*
 * This will handle connection for each client
 */
void *connection_handler(void *socket_desc)
{
	//	Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char *message , fileName[100];

	//	Receive a message from client
	while( (read_size = recv(sock , fileName , 100 , 0)) > 0 )
	{
		//	Open file and get it's size
		FILE *f = fopen(fileName, "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		//	Read file content
		char *message = malloc(fsize + 1);
		fread(message, fsize, 1, f);
		fclose(f);

		//	Set buffer as null-terminated string
		message[fsize] = 0;

		//	Send the file content back to client
		write(sock , message , strlen(message));
	}

	if(read_size == 0)
	{
		printf("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
		return (void*)-1;
	}

	//	Free the socket pointer
	free(socket_desc);
	return 0;
}