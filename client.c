#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char fileName[1000] , server_reply[2000];
	char serverIP[50];
	char exitMessage[20];

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		perror("Could not create socket\n");
		return -1;
	}
	printf("Socket created\n");

	printf("Enter server IP : ");
	scanf("%s" , serverIP);
	strcpy_s(exitMessage, 4, "exit");

	server.sin_addr.s_addr = inet_addr(serverIP);
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Connection error");
		return -1;
	}

	printf("Connected\n");

	//keep communicating with server
	while(1)
	{
		printf("Enter file name to recieve : ");
		scanf("%s" , fileName);

		if(strcasecmp(exitMessage, fileName) == 0)
		{
			printf("Bye");
			break;
		}

		//Send some data
		if( send(sock , fileName , strlen(fileName) , 0) < 0)
		{
			perror("Send failed");
			return -1;
		}

		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0)
		{
			perror("Recieve failed");
			return -1;
		}

		printf("Server reply :\n");
		printf(server_reply);
	}
	printf("\n");
	close(sock);
	return 0;
}