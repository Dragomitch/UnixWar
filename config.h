/*
 * =====================================================================================
 *
 *       Filename:  config.h
 *
 *    Description:  Header contenant toutes les constantes propres au jeu
 *
 *        Version:  1.0
 *        Created:  05/04/2016 03:54:34 PM
 *       Revision:  1
 *       Compiler:  cc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:  IPL-Student
 *
 * =====================================================================================
*/
typedef int bool;
#define TRUE 1
#define FALSE 0

//message codes
#define WAIT		0 //server -> client
#define REFUSE		1 //server -> client
#define NICKNAME	2 //client -> server
#define DISCONNECT	3 //either way
#define DEAL		4 //server -> client
#define ASK		5 //server -> client
#define PLAY		6 //client -> server
#define GIVE		7 //server -> client
#define EMPTY		8 //client -> server
#define ROUND		9 //server -> client
#define SCORE		10 //user -> client -> shared memory
#define SCORES		11 //server -> client
#define WINNER		12 // ?

//ports
#define PORT_DIMOV 	17626
#define PORT_DRAGOMIR 	18206
