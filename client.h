/*
 * =====================================================================================
 *
 *       Filename:  client.h
 *
 *    Description:  Header du  client pour le projet
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

#define NAMESIZE 20

void connectToServer(int *client_socket,char* serverIP,struct hostent *he,struct sockaddr_in *serverAddress);
void create_nickname(char *name);
void send_message(int client_socket);
void receive_message(int client_socket, char* message);
void create_nickname_shared_memory(char* nickname);