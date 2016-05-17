/*
 * =====================================================================================
 *
 *       Filename:  server_utils.h
 *
 *    Description:	Utilitary functions used by the server program
 *
 *        Version:  1.0
 *        Created:  2016-05-17 15:24:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DIMOV Theodor, DRAGOMIR Philippe
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

void extract_player_nickname( char** msg, char* nickname);
int rand_range( int upper_limit );
bool array_contains( int* haystack, int needle, int length );
