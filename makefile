#
# =====================================================================================
#
#       Filename:  makefile
#
#    Description:  fichier générant et gérant les dépendances du projet
#
#        Version:  1.0
#        Created:  05/04/2016 03:54:34 PM
#       Revision:  1
#       Compiler:  cc
#
#         Author:  DIMOV Theodor, DRAGOMIR Philippe
#   Organization:  IPL-Student
#
# =====================================================================================
#
TARGET= server client

normal: $(TARGET)

Server: Server.c 
	gcc -Wall -Wextra server.c -o Server

Client: Client.c
	gcc -Wall -Wextra client.c -o Client

clean:
	$(RM) $(TARGET)
