/*
 * =====================================================================================
 *
 *       Filename:  common_utils.c
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
#include "common_utils.h"

void send_message_(char* message, int socket) {
	if (send(socket, message, strlen(message), 0) == -1) {
		perror("Send");
		exit(EXIT_FAILURE);
	}
}

/* *
 * extracts the message code header from the rest of the message.
 *
 * message : the received message to decode
 *
 * */
int extract_message_code(char** message) {
	char* token;
	token = strtok_r(*message, " ", message);
	*message = strtok_r(*message, "", message);
	return atoi(token);
}

/* *
 * decodes the message payload
 *
 * raw_payload : the payload to decode
 * decoded_payload : area to store the decoded payload
 * max_elements : the maximum number of elements in the payload
 *
 * */
int decode_message_payload(char** raw_payload, int** decoded_payload, int max_elements) {
	if (!(*decoded_payload = (int*) malloc(max_elements * sizeof(int)))) {
		perror("malloc error");
	}
	int i;
	for (i = 0; i < max_elements; i++) {
		char* token = strtok_r(*raw_payload, " ", raw_payload );
		if (token == NULL) {
			return i;
		}
		*(*decoded_payload + i) = atoi(token);
	}
	return i;
}

void extract_player_nickname(char** message, char** nickname) {
	if (!(*nickname = (char*) malloc (20* sizeof(char)))) {
		perror("malloc");
	}
	sprintf(*nickname,"%s", strtok_r(*message, " ", message));
}

int rand_range(int upper_limit) {
	return (int) (( (double) (upper_limit+1) / RAND_MAX) * rand());
}

bool array_contains(int* haystack, int needle, int length) {
	int* array_ptr = haystack;
	for (; (array_ptr - haystack) < length; array_ptr++) {
		if (*array_ptr == needle) {
			return TRUE;
		}
	}
	return FALSE;
}
