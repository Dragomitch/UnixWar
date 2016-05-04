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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int extract_message_code( char** );
int* decode_message_payload( char**, int );

int main(int argc, char** argv) {

	char* buffer;
	if (!(buffer = (char*) malloc(256* sizeof(char)))) {
		perror("malloc error");
	}
	fgets(buffer, 256, stdin);
	int message_code = extract_message_code(&buffer);
	printf("message code : %d\n", message_code);
	int numElements = atoi(argv[1]);
	printf("ok\n");
	int* decoded_payload = decode_message_payload(&buffer, numElements);
	printf("ok\n");
	int i = 0;
	for (; i < numElements; i++) {
		printf("element n. %d : %d\n", i+1, *( decoded_payload+i));
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
	token = strtok_r(*message, " ", message); //thread safe
	*message = strtok_r(*message, "", message);
	return atoi(token);
}

/* *
 * decodes the message payload
 *
 * raw_payload : the payload to decode
 * numElements : the number of elements in the payload
 *
 * */
int* decode_message_payload(char** raw_payload, int numElements) {
	int* decoded_payload;
	if (!(decoded_payload = (int*) malloc(numElements * sizeof(int)))) {
		perror("malloc error");
	}
	int i;
	for (i = 0; i < numElements; i++) {
		*(decoded_payload + i) = atoi(strtok_r(*raw_payload, " ", raw_payload ));
	}
	return decoded_payload;
}
