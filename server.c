#include "server.h"

int cl_count;
int cl_sockets[MAX_PLAYERS];
char cl_nicknames[MAX_PLAYERS][20];
bool game_in_progress;
bool time_is_up;
bool running;
bool end_of_turn;

fct_ptr dispatcher[] = { add_player, refuse_connection, add_nickname, 0, 0, 0, receive_card, 0, end_round };

int main(int argc, char** argv) {
	int server_socket, max_fd, select_res, i;
	struct sockaddr_in my_addr, cl_addr;
	struct timeval timeout = {0, 15000}; //15 milliseconds
	fd_set fds;

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
		if ((select_res = select(max_fd, &fds, NULL, NULL, &timeout)) < 0) {
			//errno is set to EINTR when select() is interrupted by a signal, in our case, the alarm
			if (errno != EINTR) {
				return EXIT_FAILURE;
			}
		}

		if (select_res >= 0 && select_res != 0) {
			for (i = 0; i < max_fd; i++) {
				if (FD_ISSET(i, &fds)) {
					if (i == server_socket) {
						printf("going in .. \n");
						add_client(server_socket, &cl_addr);
					} else {
						receive_msg(i);
					}
				}
			}
		}
		if (game_in_progress) {
			if (end_of_turn) {
				end_of_turn = FALSE;
				broadcast(ASK, "");
			}
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
	if ((new_cl_socket = accept(server_socket, (struct sockaddr *)cl_addr, (socklen_t*) &cl_addr_length)) < 0) {
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
	printf("adding!\n");
	char countdown[2];
	sprintf(countdown, "%d", COUNTDOWN);
	send_msg(WAIT, countdown, socket);
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
	send_msg(REFUSE, "", socket);
}

void add_nickname(int socket, char** msg) {
	printf("nickname : %s\n", *msg);
	char nickname[NAMESIZE];
	extract_player_nickname(msg, nickname);
	int i;
	for (i = 0; i < cl_count; i++) {
		if (cl_sockets[i] == socket) {
			sprintf(cl_nicknames[i], "%s", nickname);
			printf("added a player nickname : %s\n", cl_nicknames[i]);
			break;
		}
	}
}

void deal_cards() {
	srand(time(NULL));
	int cards_per_player = DECK_SIZE / cl_count;
	int dealt_cards[cards_per_player * cl_count];
	int total_dealt_cards = 0;
	int player;
	for (player = 0; player < cl_count; player++) {
		int card;
		int str_length = 0;
		char msg[3* cards_per_player]; //2 caractèress par carte + 1 espace
		msg[0] = '\0';
		for (card = 0; card < cards_per_player; card++) {
			int random_card;
			do {
				random_card = rand_range(DECK_SIZE);
				//choisir une carte tant qu'on n'en trouve pas une qui n'a pas encore été choisie
			} while (array_contains(dealt_cards, random_card, total_dealt_cards));
			//rajouter la carte au message
			str_length += sprintf(msg+str_length, "%d ", random_card);
			dealt_cards[total_dealt_cards++] = random_card;
		}
		//distribuer les cartes choisies au joueur
		send_msg(DEAL, msg, cl_sockets[player]);
		printf("cards dealt : \n");
		printf("%s\n", msg);
	}
}

void clear_lobby() {
	broadcast(DISCONNECT, "");
	game_in_progress = FALSE;
	int i;
	for (i = 0; i < cl_count; i++) {
		if (cl_sockets[i] > 0) {
			remove_player(i);
		}
	}
}

void broadcast(int msg_code, char* payload) {
	char msg[MESSAGE_SIZE];
	sprintf(msg, "%d %s", msg_code, payload);
	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (cl_sockets[i] != 0) {
			if (send(cl_sockets[i], msg, MESSAGE_SIZE, 0) == -1) {
				perror("Send");
				exit(EXIT_FAILURE);
			}
		}
	}
}

void receive_msg(int fd) {
	char msg[MESSAGE_SIZE];
	char* msg_ptr = msg;
	int bytes_received;
	if ((bytes_received = recv(fd, msg, MESSAGE_SIZE, 0)) <= 0) {
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
		printf("message : %s\n", msg_ptr);
		int msg_code = extract_msg_code(&msg_ptr);
		dispatcher[msg_code] (fd, &msg_ptr);
	}
}

void start_game() {
	broadcast(ROUND, "");
	deal_cards();
	start_round();
	game_in_progress = TRUE;
}

void start_round() {
	end_of_turn = FALSE;
	broadcast(ASK, "");
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
	//free shared memory (allocate some first)
}

void receive_card(int socket, char** msg) {
	static int received_cards_count = 0;
	static int highest_card = -1;
	static int highest_card_holder = -1;
	static int str_length = 0;
	static char cards[MAX_PLAYERS * 3];
	int card_container[1];
	int* card = card_container;
	decode_msg_payload(msg, card, 1);
	str_length += sprintf(cards+str_length, "%d ", *card);
	int i;
	for ( i = 0; i < cl_count; i++) {
		if (cl_sockets[i] == socket) {
			printf("%s played the %s with id %d\n", cl_nicknames[i], get_card_name(*card), *card);
			if (*card > highest_card) {
				highest_card = *card;
				highest_card_holder = i;
			}
		}
	}
	received_cards_count++;
	if (received_cards_count == cl_count) {
		send_msg(GIVE, cards, cl_sockets[highest_card_holder]);
		printf("player %s wins : %s\n", cl_nicknames[highest_card_holder], cards);
		received_cards_count = 0;
		highest_card = -1;
		highest_card_holder = -1;
		memset(cards, 0, cl_count * sizeof(int));
		str_length = 0;
		end_of_turn = TRUE;
	}
	char buffer[10];
	fgets(buffer, 10, stdin);
}

void end_round(int socket, char** msg) {
	printf("end of round !\n");
}
