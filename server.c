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
#define BACKLOG 20
#define COUNTDOWN 30 //30 seconds wait time

#define TRUE 1
#define FALSE 0

#define MAX_PLAYERS 4

typedef int bool;

int num_clients_connected;
int round_number;
bool game_in_progress;
bool time_is_up;

void alarm_handler(int signum) {
	printf("alarm time !!\n");
	if (signum == SIGALRM) {
		time_is_up = TRUE;
		alarm(0);
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
			perror("Could not read message : ");
		}
			close(fd);
			FD_CLR(fd,fds);
	}
}

void add_new_client_socket(fd_set *fds, int *max_fd, int server_socket, struct sockaddr_in *client_address) {
	int new_client_socket;
	int client_address_length = sizeof(struct sockaddr_in);
	if ((new_client_socket = accept(server_socket, (struct sockaddr *)client_address, (socklen_t*) &client_address_length)) == -1) {
		perror("Connection error : ");
		exit(EXIT_FAILURE);
	} else {

		if (!game_in_progress && num_clients_connected < 4) {
			printf("new player connected, %d players in total\n", num_clients_connected);
			if (num_clients_connected == 0) {
				//first client, set an alarm for 30 seconds
				struct sigaction action;
				action.sa_handler = alarm_handler;
				sigaction(SIGALRM, &action, NULL);
				alarm(COUNTDOWN);
			} else if (num_clients_connected == 4) {
				alarm(0);
			}
			FD_SET(new_client_socket, fds);
			if (new_client_socket >= *max_fd) {
				*max_fd = new_client_socket+1;
				num_clients_connected++;
			}
			char message[5];
			sprintf(message, "%d %d", WAIT, COUNTDOWN);
			if (send(new_client_socket, message, sizeof(message), 0) == -1) {
				perror("Send : ");
			}
			printf("new connection accept + %d: %s\n", WAIT, inet_ntoa(client_address->sin_addr));


		} else {
			char message[2];
			sprintf(message, "%d", REFUSE);
			if (send(new_client_socket, message, sizeof(message), 0) == -1) {
				perror("Send : ");
			}
			printf("refuse connection %d\n", REFUSE);
		}
	}
}

void init_server_socket(int *server_socket,struct sockaddr_in *my_addr) {

	int yes=1;
	if ((*server_socket = socket(PF_INET,SOCK_STREAM, 0)) == -1) {
		perror("Serveur: socket ");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(*server_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("Serveur: setsockopt");
		exit(EXIT_FAILURE);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(PORT);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr->sin_zero), '\0', 8);

	if (bind(*server_socket, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Serveur: bind");
		exit(EXIT_FAILURE);
	}

	if (listen(*server_socket, BACKLOG) == -1) {
		perror("Serveur: listen");
		exit(EXIT_FAILURE);
	}
	fflush(stdout);//on vide le buffer
}

void notify_game_over(fd_set* fds, int* max_fd, int server_socket) {
	printf("notifying ..\n");
	//TODO send a message to every waiting client to inform them that the game will not start
	FD_ZERO(fds);
	FD_SET(server_socket, fds);
	*max_fd = server_socket + 1;
}

void start_game(fd_set* fds, int* max_fd, int server_socket) {
	printf("game starting!\n");
	//TODO perform necessary treatments to start the game
	game_in_progress = TRUE;
}

void reset_fd(int fd, fd_set* fds) {
	printf("resetting fd %d\n", fd);
	FD_CLR(fd, fds);
	FD_SET(fd, fds);
}

int main() {
	int server_socket, fd, max_fd;
	struct sockaddr_in my_addr, client_addr;
	fd_set readfds, fds;

	init_server_socket(&server_socket, &my_addr);
	FD_ZERO(&fds);
	FD_SET(server_socket, &fds);
	max_fd = server_socket + 1;

	num_clients_connected = 0;
	round_number = 0;
	game_in_progress = FALSE;
	time_is_up = FALSE;

	while (TRUE) {
    	readfds=fds;
		if ((select(max_fd, &readfds, NULL, NULL, NULL)) < 0) {
			//errno is set to EINTR when select() is interrupted by a signal, in our case, the alarm
			if (errno != EINTR) {
				return EXIT_FAILURE;
			}
		}
		for (fd = 0; fd < max_fd; fd++) {
			if (FD_ISSET(fd, &readfds)) {
				if (fd == server_socket) {
    				add_new_client_socket(&fds, &max_fd, server_socket, &client_addr);
				}
				else {
					receive_message(fd, &fds);
				}
			}
			reset_fd(fd, &readfds);
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
		}
	}
	close(server_socket); //tester la valeur de retour
	return EXIT_SUCCESS;
}
