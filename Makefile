TARGET= server client

normal: $(TARGET)

server: server.o common_utils.o server_utils.o cards.o
	gcc -Wall -Wextra server.o common_utils.o server_utils.o cards.o -o server

client: client.o common_utils.o cards.o
	gcc -Wall -Wextra client.o common_utils.o cards.o -o client

server.o: server.c common_utils.h server_utils.h cards.h
	gcc -Wall -Wextra -c server.c

client.o: client.c common_utils.h cards.h
	gcc -Wall -Wextra -c client.c

common_utils.o:	common_utils.h common_utils.c
	gcc -c common_utils.c

server_utils.o: server_utils.h server_utils.c
	gcc -c server_utils.c

cards.o: cards.h cards.c
	gcc -c cards.c

clean:
	$(RM) $(TARGET)
