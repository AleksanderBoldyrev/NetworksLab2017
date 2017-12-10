#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>

#include <netdb.h>
#include "ClientWorker.h"

int main(int argc, void** argv)
{
	setlocale(LC_ALL, "en_US.UTF-8");
	ClientWorker f;
	printf("Enter <host>%c<port> of Mail Server: ", DELIM);
	char s[1024];
	scanf("%s", s);
	string* ff = new string(s);
	f.StartThread(ff);
        printf("Mail client terminated!\n");
        getchar();
	delete ff;
	return 0;
}

