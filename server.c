#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define MYPORT 5555
#define BUFFERSIZE 1000
#define BACKLOG 20

void sendToAll(int i, int j, int serverSocket,fd_set *fds,int bytesReceived,char* messageFromClient){
	printf("coucou");
	if (FD_ISSET(j,fds)){
		if (j!=serverSocket && j!=i){
			if (send(j,messageFromClient,bytesReceived,0)==-1)
				perror("send");
		}
	}
}


void receiveMessage(int i,fd_set *fds,int serverSocket,int maxFD){
	char messageFromClient[BUFFERSIZE];
	int bytesReceived,j;
	if ((bytesReceived=recv(i,messageFromClient,2*BUFFERSIZE,0))<=0){
		if (bytesReceived==0){
			printf("Client hung up");
		}
		else{
			perror("recv ");
		}
			close(i);
			FD_CLR(i,fds);
	}
	else{
		for (j=0;j<maxFD;j++){
			sendToAll(i,j,serverSocket,fds,bytesReceived,messageFromClient);
		}
	}
}


void addNewClientSocket(fd_set *fds, int *maxFD, int serverSocket, struct sockaddr_in *clientAddress){
	int newClientSocket,clientAddressLength=sizeof(struct sockaddr_in);
	if ((newClientSocket=accept(serverSocket,(struct sockaddr *)clientAddress,&clientAddressLength))==-1){
		perror("new client accept : ");
		exit(EXIT_FAILURE);
	}
	else{
		FD_SET(newClientSocket,fds);
		if (newClientSocket>=*maxFD)
			*maxFD=newClientSocket+1;
		printf("new connection : %s\n",inet_ntoa(clientAddress->sin_addr));
	}
}

void initServerSocket(int *serverSocket,struct sockaddr_in *my_addr){

	int yes=1;
	if ((*serverSocket = socket(PF_INET,SOCK_STREAM, 0)) == -1) {
		perror("Serveur: socket ");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(*serverSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("Serveur: setsockopt");
		exit(EXIT_FAILURE);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(MYPORT);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr->sin_zero), '\0', 8);

	if (bind(*serverSocket, (struct sockaddr *)my_addr,sizeof(struct sockaddr)) == -1) {
		perror("Serveur: bind");
		exit(EXIT_FAILURE);
	}

	if (listen(*serverSocket, BACKLOG) == -1) {
		perror("Serveur: listen");
		exit(EXIT_FAILURE);
	}
	fflush(stdout);//on vide le buffer
}

int main(){
	int serverSocket,new_fd,j,i,maxFD;
	struct sockaddr_in my_addr,clientAddress;
	fd_set readfds,fds;

	initServerSocket(&serverSocket,&my_addr);

	FD_ZERO(&fds);
	FD_SET(serverSocket,&fds);
	maxFD=serverSocket+1;

	while (1){
    readfds=fds;
		if (select(maxFD,&readfds,NULL,NULL,NULL)==-1){
			perror("select : ");
			return EXIT_FAILURE;
		}

		for (i=0;i<maxFD;i++){
			if (FD_ISSET(i, &readfds)){
				if (i==serverSocket){
    			addNewClientSocket(&fds,&maxFD,serverSocket,&clientAddress);
				}
				else{
					receiveMessage(i,&fds,serverSocket,maxFD);
				}
			}
		}
	}
	close(serverSocket);
	return EXIT_SUCCESS;
}
