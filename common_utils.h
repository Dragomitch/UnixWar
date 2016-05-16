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

#define MESSAGE_SIZE 82
void send_msg( int, const char*, int );
int extract_msg_code( char** );
int decode_msg_payload( char**, int*, int );
void extract_player_nickname( char**, char* );
int rand_range( int );
bool array_contains( int*, int, int );

#endif
