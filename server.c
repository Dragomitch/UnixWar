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
#include "config.h"

#define PORT 5555 //TODO mettre le port correct
#define BUFFER_SIZE 1024
#define BACKLOG 5
#define COUNTDOWN 10 //30 seconds wait time

#define TRUE 1
#define FALSE 0

#define MAX_PLAYERS 4

typedef int bool;

int num_clients_connected;
char client_sockets[MAX_PLAYERS];
int round_number;
bool game_in_progress;
bool time_is_up;

void alarm_handler(int signum) {
	printf("alarm time !!\n");
	if (signum == SIGALRM) {
		time_is_up = TRUE;
	}
}

void receive_message(int fd, fd_set *fds) {
	char client_message[BUFFER_SIZE];
	int bytes_received;
	if ((bytes_received = recv(fd, client_message, BUFFER_SIZE, 0)) <= 0) {
		if (bytes_received == 0) {
			printf("Client disconnected.\n");
		}
		else {
			perror("Could not read message");
		}
			close(fd);
			//FD_CLR(fd,fds); pointless since it is zeroed on every loop turn
			int i;
			for (i = 0; i < num_clients_connected; i++) {
				if (client_sockets[i] == fd) {
					int j;
					for (j = i+1; j < num_clients_connected; j++) {
						client_sockets[j-1] = client_sockets[j];
					}
					break;
				}
			}
			num_clients_connected--;
	} else {
		printf("%s\n", client_message);
	}
	fflush(stdout);
	fflush(stdin);
}

void add_new_client_socket(fd_set *fds, int *max_fd, int server_socket, struct sockaddr_in *client_address) {
	int new_client_socket;
	int client_address_length = sizeof(struct sockaddr_in);
	if ((new_client_socket = accept(server_socket, (struct sockaddr *)client_address, (socklen_t*) &client_address_length)) == -1) {
		perror("Connection error");
		exit(EXIT_FAILURE);
	} else {

		if (!game_in_progress && num_clients_connected < 4) {
			printf("new player connected, %d players in total\n", num_clients_connected+1);
			if (num_clients_connected == 0) {
				//first client, set an alarm for 30 seconds
				struct sigaction action;
				action.sa_handler = &alarm_handler;
				sigaction(SIGALRM, &action, NULL);
				alarm(COUNTDOWN);
			} else if (num_clients_connected == 4) {
				alarm(0);
			}
			FD_SET(new_client_socket, fds);
			client_sockets[num_clients_connected] = new_client_socket;
			if (new_client_socket > *max_fd) {
				*max_fd = new_client_socket+1;
			}
			num_clients_connected++;
			char message[5];
			sprintf(message, "%d %d", WAIT, COUNTDOWN);
			if (send(new_client_socket, message, sizeof(message), 0) == -1) {
				perror("Send");
			} else {
				printf("Message sent !!!\n");
			}
			printf("new connection accept + %d: %s\n", WAIT, inet_ntoa(client_address->sin_addr));

		} else {
			char message[2];
			sprintf(message, "%d", REFUSE);
			if (send(new_client_socket, message, sizeof(message), 0) == -1) {
				perror("Send");
			}
			printf("refuse connection %d\n", REFUSE);
		}
	}
}

void init_server_socket(int *server_socket,struct sockaddr_in *my_addr) {

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

	if (listen(*server_socket, BACKLOG) == -1) {
		perror("Listen");
		exit(EXIT_FAILURE);
	}
	fflush(stdout);//on vide le buffer
}

void notify_game_over(fd_set* fds, int* max_fd, int server_socket) {
	printf("notifying ..\n");
	char message[2];
	sprintf(message, "%d", DISCONNECT);
	int i;
	for (i = 0; i < num_clients_connected; i++) {
		if (send(client_sockets[i], message, sizeof(message), 0) == -1) {
			perror("Send");
		}
		client_sockets[i] = 0;
	}
	num_clients_connected = 0;
	time_is_up = FALSE;
}

void start_game(fd_set* fds, int* max_fd, int server_socket) {
	printf("game starting!\n");
	//TODO perform necessary treatments to start the game
	game_in_progress = TRUE;
}

int main() {
	int server_socket, fd, max_fd, i;
	struct sockaddr_in my_addr, client_addr;
	fd_set fds;

	init_server_socket(&server_socket, &my_addr);

	for (i = 0; i < MAX_PLAYERS; i++) {
		client_sockets[i] = 0;
	}

	num_clients_connected = 0;
	round_number = 0;
	game_in_progress = FALSE;
	time_is_up = FALSE;

	while (TRUE) {
		FD_ZERO(&fds);
		FD_SET(server_socket, &fds);
		max_fd = server_socket + 1;
		int i;
		for (i = 0; i < MAX_PLAYERS; i++) {
			if (client_sockets[i] != server_socket) {
				FD_SET(client_sockets[i], &fds);
			}
			if (client_sockets[i] > max_fd) {
				max_fd = client_sockets[i]+1;
			}
		}
		if ((select(max_fd, &fds, NULL, NULL, NULL)) < 0) {
			//errno is set to EINTR when select() is interrupted by a signal, in our case, the alarm
			if (errno != EINTR) {
				return EXIT_FAILURE;
			}
		}
		if (FD_ISSET(server_socket, &fds)) {
			printf("socket set (master) : %d\n", server_socket);
			add_new_client_socket(&fds, &max_fd, server_socket, &client_addr);
		}

		for (fd = 0; fd < num_clients_connected; fd++) {
			printf("reading what the dumbass has to say.. \n");
			if (FD_ISSET(client_sockets[fd], &fds)) {
				printf("socket set : %d at index %d\n", client_sockets[fd], fd);
				receive_message(client_sockets[fd], &fds);
			}
		}

		if (!game_in_progress && time_is_up) {
			if (num_clients_connected < 2) {
				notify_game_over(&fds, &max_fd, server_socket);
			} else {
				start_game(&fds, &max_fd, server_socket);
			}
			time_is_up = FALSE;
		} else if (!game_in_progress && num_clients_connected == MAX_PLAYERS) {
			start_game(&fds, &max_fd, server_socket);
			alarm(0);
		}
	}
	close(server_socket); //tester la valeur de retour
	return EXIT_SUCCESS;
}
