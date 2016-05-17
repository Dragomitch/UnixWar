int hand[DECK_SIZE / 2];
int stash[DECK_SIZE / 2];
int cards_in_hand;
int cards_in_stash;

void receive_msg(int i, int socket, char** nickname) {
	char msg[MESSAGE_SIZE];
	char* msg_ptr = msg; // !!
	int bytes_received;
	if ((bytes_received=recv(socket,msg_ptr,MESSAGE_SIZE,0))==-1){
			perror("Client receive from server : ");
			exit(EXIT_FAILURE);
	}
	msg[bytes_received]='\0';
	int msg_code = extract_msg_code(&msg_ptr); //renvoie le code et déplace le pointeur
	if (msg_code == WAIT) {
		printf("received a connection confirmation WAIT\n");
		printf("my nickname : %s\n", *name);
		send_msg(NICKNAME, *name, clientSocket);
	} else if (msg_code == REFUSE || msg_code == DISCONNECT) {
		printf("received a connection refusal REFUSE\n");
		close(clientSocket);
	} else if (msg_code == ROUND) {
		printf("new round\n");
	} else if (msg_code == DEAL) {
		int dealt_cards_array[DECK_SIZE / 2];
		int* dealt_cards = dealt_cards_array;
		int size = decode_msg_payload(&msg, dealt_cards, DECK_SIZE / 2); //remplit la liste de cartes et renvoie le nombre de cartes reçues
		memcpy(hand, dealt_cards, size * sizeof(int));
		cards_in_hand = size;
	} // ...
}