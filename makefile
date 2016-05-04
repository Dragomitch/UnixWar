TARGET= server client

normal: $(TARGET)

chatRoomServer: ChatRoomServer.c
	gcc -Wall -Wextra server.c -o Server

chatRoomClient: ChatRoomClient.c
	gcc -Wall -Wextra client.c -o Client

clean:
	$(RM) $(TARGET)
