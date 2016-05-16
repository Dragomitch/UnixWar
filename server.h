/*
 * =====================================================================================
 *
 *       Filename:  server.h
 *
 *    Description:	
 *
 *        Version:  1.0
 *        Created:  2016-05-15 12:13:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:  IPL
 *
 * =====================================================================================
 */

#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "common_utils.h"
#include "cards.h"

#define MIN_PLAYERS 2
#define PORT PORT_DIMOV
#define BUFFER_SIZE 1024
#define BACKLOG 5
#define COUNTDOWN 10 //30 seconds wait time

typedef void (*fct_ptr)( );

void init_server(int*, struct sockaddr_in*);
void alarm_handler(int);
void interrupt_handler(int);
void shutdown_socket(int);
void shutdown_server();
void broadcast(int, char*);
void add_client(int, struct sockaddr_in*);
void add_player(int); //accept connection
void remove_player(int);
void refuse_connection(int);
void receive_msg(int);
void clear_lobby();
void add_nickname(int, char**);
void start_game();
void deal_cards();
void start_round();
void receive_card(int, char**);
void end_round(int, char**);
