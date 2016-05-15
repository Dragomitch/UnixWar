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

void send_message_(char*, int);
int extract_message_code( char** );
int decode_message_payload( char**, int**, int );
void extract_player_nickname( char**, char** );
int rand_range(int);
bool array_contains(int*, int, int);

#endif
