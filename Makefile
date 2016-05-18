TARGET= server client

CC=gcc

normal: $(TARGET)

server: server.o common_utils.o server_utils.o cards.o
	$(CC) -Wall -Wextra server.o common_utils.o server_utils.o cards.o -o server

client: client.o common_utils.o cards.o
	$(CC) -Wall -Wextra client.o common_utils.o cards.o -o client

server.o: server.c common_utils.h server_utils.h cards.h
	$(CC) -Wall -Wextra -c server.c

client.o: client.c common_utils.h cards.h
	$(CC) -Wall -Wextra -c client.c

common_utils.o:	common_utils.h common_utils.c
	$(CC) -Wall -Wextra -c common_utils.c

server_utils.o: server_utils.h server_utils.c
	$(CC) -Wall -Wextra -c server_utils.c

cards.o: cards.h cards.c
	$(CC) -Wall -Wextra -c cards.c

clean:
	$(RM) $(TARGET)
