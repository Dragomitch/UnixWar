#include "server.h"

int cl_count;
int cl_sockets[MAX_PLAYERS];
char cl_nicknames[MAX_PLAYERS][20];
bool game_in_progress;
bool time_is_up;
bool running;

fct_ptr dispatcher[] = { add_player, refuse_connection, add_nickname };

int main(int argc, char** argv) {
	int server_socket, max_fd, i;
	struct sockaddr_in my_addr, cl_addr;
	fd_set fds;
	char ask_msg[3]; //les codes des messages prennen au maximum 2 caractères
	sprintf(ask_msg, "%d", ASK);

	for (i = 0; i < MAX_PLAYERS; i++) {
		cl_sockets[i] = 0;
	}

	cl_count = 0;
	game_in_progress = FALSE;
	time_is_up = FALSE;
	running = TRUE;

	struct sigaction alarm, interrupt;
	alarm.sa_handler = &alarm_handler;
	interrupt.sa_handler = &interrupt_handler;
	sigaction(SIGALRM, &alarm, NULL);
	sigaction(SIGINT, &interrupt, NULL);

	init_server(&server_socket, &my_addr);

	while (running) {
		FD_ZERO(&fds);
		FD_SET(server_socket, &fds);
		max_fd = server_socket + 1;
		int i;
		for (i = 0; i < MAX_PLAYERS; i++) {
			if (cl_sockets[i] > 0) {
				FD_SET(cl_sockets[i], &fds);
			}
			if (cl_sockets[i] >= max_fd) {
				max_fd = cl_sockets[i]+1;
			}
		}
		printf("number of clients : %d\n", cl_count);
		printf("game in progress : %d\n", game_in_progress);
		if ((select(max_fd, &fds, NULL, NULL, NULL)) < 0) {
			//errno is set to EINTR when select() is interrupted by a signal, in our case, the alarm
			if (errno != EINTR) {
				return EXIT_FAILURE;
			}
		}
		if (FD_ISSET(server_socket, &fds)) {
			add_client(server_socket, &cl_addr);
		}

		for (i = 0; i < cl_count; i++) {
			if (FD_ISSET(cl_sockets[i], &fds)) {
				receive_message(cl_sockets[i]);
			}
		}
		if (game_in_progress) {
			broadcast(ask_msg); //demander aux joueurs de jouer leur carte
		}
	}
	shutdown_socket(server_socket);
	return EXIT_SUCCESS;
}

void alarm_handler(int signum) {
	if (signum == SIGALRM) {
		if (cl_count < 2) {
			clear_lobby();
		} else {
			start_game();
		}
		time_is_up = TRUE;
	}
}

void interrupt_handler(int signum) {
	if (signum == SIGINT) {
		shutdown_server();
	}
}

void init_server(int *server_socket,struct sockaddr_in *my_addr) {

	if ((*server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(EXIT_FAILURE);
	}

	memset(my_addr, 1, sizeof(struct sockaddr_in));
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(PORT);
	my_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	//memset(&(my_addr->sin_zero), '\0', 8);

	if (bind(*server_socket, (struct sockaddr *)my_addr, sizeof(*my_addr)) == -1) {
		perror("Bind");
		exit(EXIT_FAILURE);
	}

	if (listen(*server_socket, BACKLOG)) {
		perror("Listen");
		exit(EXIT_FAILURE);
	}
}

void add_client(int server_socket, struct sockaddr_in *cl_addr) {
	int new_cl_socket;
	int cl_addr_length = sizeof(struct sockaddr_in);
	if ((new_cl_socket = accept(server_socket, (struct sockaddr *)cl_addr, (socklen_t*) &cl_addr_length)) == -1) {
		perror("Connection error");
		exit(EXIT_FAILURE);
	} else {
		//either accept or refuse the connection
		dispatcher[(game_in_progress || cl_count == MAX_PLAYERS)] (new_cl_socket) ;
		//le résultat de la condition booléenne sert comme indice du tableau pour appeler la bonne fonction et lui passer le socket en paramètre
	}
}

void add_player(int socket) {
	cl_sockets[cl_count++] = socket;
	char message[5];
	sprintf(message, "%d %d", WAIT, COUNTDOWN);
	send_message_(message, socket);
	if (cl_count == 1) {
		//first client, set an alarm for 30 seconds
		alarm(COUNTDOWN);
	}
}

void remove_player(int index) {
	shutdown_socket(cl_sockets[index]);
	memcpy(cl_nicknames[index], "\0", 20);
	cl_sockets[index] = 0;
	cl_count--;
}

void refuse_connection(int socket) {
	char message[2];
	sprintf(message, "%d", REFUSE);
	send_message_(message, socket);
}

void add_nickname(int socket, char** message) {
	printf("nickname : %s\n", *message);
	char* nickname;
	extract_player_nickname(message, &nickname);
	int i;
	for (i = 0; i < cl_count; i++) {
		if (cl_sockets[i] == socket) {
			sprintf(cl_nicknames[i], "%s", nickname);
			printf("added a player nickname : %s\n", cl_nicknames[i]);
			break;
		}
	}
	free(nickname);
}

void deal_cards() {
	srand(time(NULL));
	int cards_per_player = DECK_SIZE / cl_count;
	int dealt_cards[cards_per_player * cl_count];
	int total_dealt_cards = 0;
	char tmp[3];
	int player;
	for (player = 0; player < cl_count; player++) {
		int card;
		//prepare message head
		sprintf(tmp, "%d ", DEAL);
		char message[strlen(tmp) + 3* cards_per_player]; //2 caractèress par carte + 1 espace
		message[0] = '\0';
		strcat(message, tmp);
		//prepare message body
		for (card = 0; card < cards_per_player; card++) {
			int random_card;
			do {
				random_card = rand_range(DECK_SIZE);
				//choisir une carte tant qu'on n'en trouve pas une qui n'a pas encore été choisie
			} while (array_contains(dealt_cards, random_card, total_dealt_cards));
			//rajouter la carte au message
			sprintf(tmp, "%d ", random_card);
			strcat(message, tmp);
			dealt_cards[total_dealt_cards++] = random_card;
		}
		//distribuer les cartes choisies au joueur
		send_message_(message, cl_sockets[player]);
		printf("cards sent : \n");
		printf("%s\n", message);
	}
}

void clear_lobby() {
	char message[2];
	sprintf(message, "%d", DISCONNECT);
	broadcast(message);
	game_in_progress = FALSE;
	int i;
	for (i = 0; i < cl_count; i++) {
		if (cl_sockets[i] > 0) {
			remove_player(i);
		}
	}
}

void broadcast(char* message) {
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (cl_sockets[i] != 0) {
			send_message_(message, cl_sockets[i]);
		}
	}
}

void receive_message(int fd) {
	char* message;
	if (!(message = (char*) malloc(BUFFER_SIZE * sizeof(char)))) {
		perror("malloc");
	}
	char* backup = message;
	int bytes_received;
	if ((bytes_received = recv(fd, message, BUFFER_SIZE, 0)) <= 0) {
		if (bytes_received == 0) {
			printf("Client disconnected.\n");
		}
		else {
			perror("Could not read message");
		}
			int i;
			for (i = 0; i < cl_count; i++) {
				if (cl_sockets[i] == fd) {
					remove_player(i);
					int j;
					for (j = i+1; j <= cl_count; j++) { //on vient de supprimer un joueur, cl_count a été décrémenté --> '<='
						cl_sockets[j-1] = cl_sockets[j];
						sprintf(cl_nicknames[j-1], "%s", cl_nicknames[j]);
					}
					break;
				}
			}
	} else {
		dispatcher[extract_message_code(&message)] (fd, &message);
	}
	free(backup);
}

void start_game() {
	printf("game starting!\n");
	char message[3];
	sprintf(message, "%d", ROUND);
	broadcast(message);
	deal_cards();
	game_in_progress = TRUE;
}

void shutdown_socket(int socket) {
	printf("Shutting down socket number %d\n", socket);
	if (shutdown(socket, SHUT_RDWR) < 0) {
		perror("Socket shutdown");
		exit(EXIT_FAILURE);
	}
}

void shutdown_server() {
	printf("server shutting down ..\n");
	clear_lobby();
	running = FALSE;
	//free shared memory
}
