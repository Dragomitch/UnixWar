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

void send_msg(int msg_code, const char* payload, int socket) {
	char msg[MESSAGE_SIZE];
	sprintf(msg, "%d %s", msg_code, payload);
	printf("sending msg : %s\n", msg);
	if (send(socket, msg, MESSAGE_SIZE, 0) == -1) {
		perror("Send");
		exit(EXIT_FAILURE);
	}
}

/* *
 * extracts the message code header from the rest of the message.
 *
 * msg : the received message to decode
 *
 * */
int extract_msg_code(char** msg) {
	char* token;
	token = strtok_r(*msg, " ", msg);
	*msg = strtok_r(*msg, "", msg);
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
int decode_msg_payload(char** raw_payload, int* decoded_payload, int max_elements) {
	int i;
	for (i = 0; i < max_elements; i++) {
		char* token = strtok_r(*raw_payload, " ", raw_payload );
		if (token == NULL) {
			return i;
		}
		*(decoded_payload + i) = atoi(token);
	}
	return i;
}

void extract_player_nickname(char** msg, char* nickname) {
	sprintf(nickname,"%s", strtok_r(*msg, " ", msg));
}

int rand_range(int upper_limit) {
	return (int) (( (double) upper_limit / RAND_MAX) * rand());
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
