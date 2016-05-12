/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  Fichier gérant le client du projet
 *
 *        Version:  1.0
 *        Created:  05/04/2016 03:54:34 PM
 *       Revision:  1
 *       Compiler:  cc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:  IPL-Student
 *
 * =====================================================================================
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
#include "config.h"
#include "client.h"
#include "game.h"
#endif

#define BUFFERSIZE 1000

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
	printf("ok\n");
	char* request="Entrer votre pseudonyme (20 lettres): ";
	int valid=0;
	printf("%s",request);
	name = fgets(name, NAMESIZE, stdin);
	printf("ok1\n");
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
	serverAddress->sin_port = htons(PORT_DIMOV);
	serverAddress->sin_addr = *((struct in_addr*)he->h_addr);
	memset(&(serverAddress->sin_zero), '\0', 8);

	if (connect(*client_socket, (struct sockaddr *)serverAddress,sizeof(struct sockaddr)) == -1) {
		perror("Client: connect");
		exit(EXIT_FAILURE);
	}

}

int main(int argc, char *argv[]){
	if (argc != 3) {
		fprintf(stderr, "Usage : Nom_serveur port\n");
		return EXIT_FAILURE;
	}
	int client_socket,maxFD,i;
	struct sockaddr_in serverAddress;// adresse du server
	struct hostent *he;
	fd_set fds,readfds;
	char name[20];
	int port = atoi(argv[2]);

	create_nickname(name);
	if( port != PORT_DRAGOMIR && port != PORT_DIMOV){
		fprintf(stderr, "Le port devrait être l'un de ceux attribués dans le header");
		return EXIT_FAILURE;
	}

	connectToServer(&client_socket,argv[1],he,&serverAddress);
	char* message_connection;
	receive_message(client_socket, message_connection);
	strtok(message_connection, " ");
	if( message_connection  [0] == DISCONNECT){
		// Le serveur n'accepte pas notre raison pour une quelconque raison
		printf("La connexion a été refusée par le serveur.\n...Fin du programme...\n");
	}

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
