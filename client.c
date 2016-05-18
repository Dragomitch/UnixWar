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
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:  IPL-Student
 *
 * =====================================================================================
 */
#ifndef CLIENT_H
#define CLIENT_H

#include "config.h"
#include "client.h"
#endif

#define BUFFERSIZE 1000

typedef int bool;
typedef int semaphore;

int hand[DECK_SIZE];
int stash[DECK_SIZE];
int cards_in_hand;
int cards_in_stash;

void print_cards() {
	int i;
	printf("current hand : ");
	for (i = 0; i < cards_in_hand; i++) {
		printf("%d ", hand[i]);
	}
	printf("\n");
	printf("current stash : ");
	for (i = 0; i < cards_in_stash; i++) {
		printf("%d ", stash[i]);
	}
	printf("\n");
}

int calculate_score() {
	printf("CALCULATING SCORE!\n");
	print_cards();
	int score = 0;
	int i;
	for (i = 0; i < cards_in_hand; i++) {
		score += get_card_points(hand[i]);
	}
	for (i = 0; i < cards_in_stash; i++) {
		score += get_card_points(stash[i]);
	}
	printf("\\CALCULATING SCORE!\n");
	return score;
}

void receive_message(int clientSocket,char** name){
	char msgFromServer[MESSAGE_SIZE];
	char* msg = msgFromServer;

	int bytesReceived;
	if ((bytesReceived=recv(clientSocket,msgFromServer,MESSAGE_SIZE,0))==-1){
		perror("Client receive from server : ");
		exit(EXIT_FAILURE);
	}
	msgFromServer[bytesReceived]='\0';
	printf("%s\n",msgFromServer);
	fflush(stdout);
	int msg_code = extract_msg_code(&msg);
	printf("msg code received : %d\n", msg_code);
	if (msg_code == WAIT) {
		printf("received a connection confirmation WAIT\n");
		printf("my nickname : %s\n", *name);
		send_msg(NICKNAME, *name, clientSocket);
	} else if (msg_code == REFUSE || msg_code == DISCONNECT) {
		printf("received a connection refusal REFUSE\n");
		close(clientSocket);
	} else if (msg_code == ROUND) {
		printf("end of round!\n");
		int score = calculate_score();
		send_int_msg(SCORE, score, clientSocket);
		memset(hand, -1, DECK_SIZE * sizeof(int));
		memset(stash, -1, DECK_SIZE * sizeof(int));
		cards_in_hand = 0;
		cards_in_stash = 0;
		printf("score sent : %d .. \n", score);
	} else if (msg_code == DEAL) {
		cards_in_hand = decode_msg_payload(&msg, hand, DECK_SIZE / 2);
		print_cards();
	} else if (msg_code == ASK) {
		printf("BEFORE\n");
		print_cards();
		if (cards_in_hand + cards_in_stash == 1) {
			//the player is about to play his last card, the round is over
			send_light_msg(EMPTY, clientSocket);
		}
		if (cards_in_hand == 0) {
			//the player hasn't got any cards in his hand, his stash becomes his hand
			memcpy(hand, stash, cards_in_stash * sizeof(int));
			memset(stash, -1, cards_in_stash * sizeof(int));
			cards_in_hand = cards_in_stash;
			cards_in_stash = 0;
			printf("replenished\n");
			print_cards();
		}
		send_int_msg(PLAY, hand[0], clientSocket);
		int i;
		for (i = 0; i < cards_in_hand-1; i++) {
			hand[i] = hand[i+1];
		}
		cards_in_hand--;
		printf("AFTER\n");
		print_cards();
	} else if (msg_code == GIVE) {
		int* stash_ptr = stash + cards_in_stash;
		int size = decode_msg_payload(&msg, stash_ptr, MAX_PLAYERS);
		cards_in_stash += size;
		printf("WON CARDS!\n");
		print_cards();
		printf("\\WON CARDS!\n");
	}
}

void create_nickname(char* name){
	char* request="Entrer votre pseudonyme (20 lettres): ";
	printf("%s", request);
	scanf("%s", name);
	fflush(stdin);
	fflush(stdout);
}

void connectToServer(int *client_socket,char* server_ip,struct hostent *host,struct sockaddr_in *server_address){
	if ((host=gethostbyname(server_ip)) == NULL) {
		perror("Client: gethostbyname failed");
		exit(EXIT_FAILURE);
	}

	if ((*client_socket = socket(AF_INET,SOCK_STREAM, 0)) == -1) {
		perror("Client: socket");
		exit(EXIT_FAILURE);
	}

	server_address->sin_family = AF_INET;
	server_address->sin_port = htons(PORT_DIMOV);
	server_address->sin_addr = *((struct in_addr*)host->h_addr);
	memset(&(server_address->sin_zero), '\0', 8);

	if (connect(*client_socket, (struct sockaddr *)server_address,sizeof(struct sockaddr)) == -1) {
		perror("Client: connect");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]){
	if (argc != 2) {
		fprintf(stderr, "Usage : ./client Adresse_du_serveur\n");
		return EXIT_FAILURE;
	}

	int client_socket;
	struct sockaddr_in server_address;// adresse du server
	struct hostent *host;
	memset(hand, -1, DECK_SIZE * sizeof(int));
	memset(stash, -1, DECK_SIZE * sizeof(int));
	char name[NAMESIZE];
	char* name_ptr = name;

	create_nickname(name);
	connectToServer(&client_socket,argv[1],host,&server_address);
	
	while (1){
		receive_message(client_socket, &name_ptr);
	}
	close(client_socket);
	printf("Client - exit");
	return EXIT_SUCCESS;
}
