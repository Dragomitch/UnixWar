/*
 * =====================================================================================
 *
 *       Filename:  common_utils.h
 *
 *    Description: util functions used by the server and the client
 *
 *        Version:  1.0
 *        Created:  2016-05-02 09:20:00
 *       Revision:  none
 *       Compiler:  cc
 *
 *         Author: DIMOV Theodor, DRAGOMIR Philippe
 *
 * =====================================================================================
 */
#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "config.h"

#define MESSAGE_SIZE 82 //the maximum size a message could ever have

/*
 * sends a pre-formatted message to a given socket.
 *
 * msg: a pre-formatted message
 * socket: the receiving socket
 */
void send_prepared_msg( char* msg, int socket);

/*
 * formats and sends a full message (message code and body) to the given socket.
 *
 * msg_code: the message code
 * payload: additional payload or body
 * socket: the receiving socket
 */
void send_msg( int msg_code, const char* payload, int socket);

/*
 * formats and sends a message without a body to the given socket.
 *
 * msg_code: the message code
 * socket: the receiving socket
 */
void send_light_msg( int msg_code, int socket);

// int* ?
void send_int_msg( int msg_code, int payload, int socket);

/* *
* extracts the message code header from the rest of the message.
*
* msg : the received message to decode; the pointer is advanced to right after the message code
*
* */
int extract_msg_code( char** msg );

/* *
* decodes the message payload
*
* raw_payload: the payload to decode; the pointer is advanced to right after the last decoded element
* decoded_payload: area to store the decoded data. It should be alloca    ted and have enough space to contain all of the information necessary.
* max_elements: the maximum number of elements in the payload
*
* return: the actual number of elements found
*
* */
int decode_msg_payload( char** raw_payload, int* decoded_payload, int max_elements);

//Copié/collé du code fourni par Mr.Henriet pour l'affichage du jeu
// sequences "d'escape caracters" pour afficher les codes des cartes.

#define PIQUE  "\xE2\x99\xA0"
#define TREFLE "\xE2\x99\xA3"
#define COEUR  "\xE2\x99\xA5"
#define CARREAU  "\xE2\x99\xA6"

// un peu de couleur

#define noirTexte 30
#define rougeTexte 31
#define vertTexte 32
#define jauneTexte 33
#define bleuTexte 34
#define magentaTexte 35
#define cyanTexte 36
#define blancTexte 37

#define noirFond 40
#define rougeFond 41
#define vertFond 42
#define jauneFond 43
#define bleuFond 44
#define magentaFond 45
#define cyanFond 46
#define blancFond 47
//Copié/collé du code fourni par Mr.Henriet pour l'affichage du jeu
#endif