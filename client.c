#include "client.h"

#define PORT PORT_DIMOV

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

void sendReceive(int i,int clientSocket,char** name){
	//char msgToServer[MESSAGE_SIZE];
	char msgFromServer[MESSAGE_SIZE];
	char* msg = msgFromServer;

	int bytesReceived;
	if (i==0) { //pour envoyer un msg
	} else{ //on reçoit un msg
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
}

void createNickname(char *name){
	char* request="Enter your nickname (20 letters) : ";
	printf("%s", request);
	scanf("%s",name);
	fflush(stdin);
	fflush(stdout);
}

void connectToServer(int *clientSocket,char* serverIP,struct hostent *he,struct sockaddr_in *serverAddress){
	if ((he=gethostbyname(serverIP)) == NULL) {
		perror("Client: gethostbyname");
		exit(EXIT_FAILURE);
	}

	if ((*clientSocket = socket(AF_INET,SOCK_STREAM, 0)) == -1) {
		perror("Client: socket");
		exit(EXIT_FAILURE);
	}

	serverAddress->sin_family = AF_INET;
	serverAddress->sin_port = htons(PORT);
	serverAddress->sin_addr = *((struct in_addr*)he->h_addr);
	memset(&(serverAddress->sin_zero), '\0', 8);

	if (connect(*clientSocket, (struct sockaddr *)serverAddress,sizeof(struct sockaddr)) == -1) {
		perror("Client: connect");
		exit(EXIT_FAILURE);
	}

}

int main(int argc, char *argv[])
{
	int clientSocket,maxFD,i;
	struct sockaddr_in serverAddress;// adresse du server
	struct hostent *he;
	fd_set fds,readfds;
	memset(hand, -1, DECK_SIZE * sizeof(int));
	memset(stash, -1, DECK_SIZE * sizeof(int));
	char name[NAMESIZE];
	name[NAMESIZE-1] = '\0';
	char* name_ptr = name;
	if (argc != 2) {
		fprintf(stderr, "Donner le nom du serveur en argument.\n");
		return EXIT_FAILURE;
	}

	createNickname(name_ptr);
	connectToServer(&clientSocket,argv[1],he,&serverAddress);

	FD_ZERO(&fds);
	FD_ZERO(&readfds);
	FD_SET(0,&fds);
	FD_SET(clientSocket,&fds);
	maxFD=clientSocket+1;
	while (1){
		readfds=fds;
		if (select(maxFD,&readfds,NULL,NULL,NULL)==-1){
			perror("Client-Select : ");
			return EXIT_FAILURE;
		}
		for (i=0;i<maxFD;i++){
			if (FD_ISSET(i,&readfds)) {
				sendReceive(i,clientSocket,&name_ptr);
			}
		}
	}
	close(clientSocket);
	printf("Client - exit");
	return EXIT_SUCCESS;
}
