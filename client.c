#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include "config.h"

#define PORT 5555
#define BUFFERSIZE 1000
#define NAMESIZE 20

typedef int bool;

void receive_message(int client_socket, char* message){
	int bytesReceived;
	if ( (bytesReceived=recv(client_socket, message, BUFFERSIZE, 0) )==-1){
		perror("Client receive from server : ");
		exit(EXIT_FAILURE);
	}
	message[bytesReceived]='\0';
	printf("%s\n",message);
}

void send_message(int client_socket){
	char tempChar[BUFFERSIZE];
	fgets(tempChar, BUFFERSIZE, stdin);
	fprintf(stdin, "%s\n", tempChar);
	if (send(client_socket, tempChar,strlen(tempChar),0)==-1){
		perror("Failed to send a mesesage to the serveur");
		exit(EXIT_FAILURE);
	}
}

void create_nickname(char *name){
	char* request="Entrer votre pseudonyme (20 lettres): ";
	int valid=0;
	printf("%s",request);
	name = fgets(name, NAMESIZE, stdin);
	printf("\n");
}

void connectToServer(int *client_socket,char* serverIP,struct hostent *he,struct sockaddr_in *serverAddress){
	if ((he=gethostbyname(serverIP)) == NULL) {
		perror("Client: gethostbyname failed");
		exit(EXIT_FAILURE);
	}

	if ((*client_socket = socket(AF_INET,SOCK_STREAM, 0)) == -1) {
		perror("Client: socket");
		exit(EXIT_FAILURE);
	}

	serverAddress->sin_family = AF_INET;
	serverAddress->sin_port = htons(PORT);
	serverAddress->sin_addr = *((struct in_addr*)he->h_addr);
	memset(&(serverAddress->sin_zero), '\0', 8);

	if (connect(*client_socket, (struct sockaddr *)serverAddress,sizeof(struct sockaddr)) == -1) {
		perror("Client: connect");
		exit(EXIT_FAILURE);
	}

}

int main(int argc, char *argv[])
{
	int client_socket,maxFD,i;
	struct sockaddr_in serverAddress;// adresse du server
	struct hostent *he;
	fd_set fds,readfds;
	char name[20];

	if (argc != 2) {
		fprintf(stderr, "Donner le nom du serveur en argument.\n");
		return EXIT_FAILURE;
	}

	create_nickname(name);
	connectToServer(&client_socket,argv[1],he,&serverAddress);

	FD_ZERO(&fds);
	FD_ZERO(&readfds);
	FD_SET(0,&fds);
	FD_SET(client_socket,&fds);
	maxFD=client_socket+1;
	while (1){
		readfds=fds;
		if (select(maxFD,&readfds,NULL,NULL,NULL)==-1){
			perror("Client-Select : ");
			return EXIT_FAILURE;
		}
		for (i = 0; i < maxFD ; i++){
			if (FD_ISSET(i,&readfds))
				//receive_message(client_socket,name);
				printf("JepasseIci\n");
				send_message(client_socket);
		}
	}
	close(client_socket);
	printf("Client - exit");
	return EXIT_SUCCESS;
}
