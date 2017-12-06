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

	f.startThread(ff);
	//Sleep(5000); // wait 5 seconds until client is connecting to server
	/*while (!t.try_lock())
	{
		Sleep(1000);
	}
	t.unlock();*/
	printf("Mail client terminated!\n");
	//system("pause");
        getchar();
	delete ff;
	return 0;
}

/*
using namespace::std;

int main(int argc, char *argv[]) {
    int sockfd, n;
    uint16_t portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    string name;

    string buffer;//[256];
    //char *p = buffer;

    name = "192.168.0.200";
    portno = 5555;
    
    //cout << "Enter the host name: ";
    //cin >> name;
    //cout << "Enter the port: ";
    //cin >> portno;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(name.c_str());//argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);


    //printf("Please enter the message: ");
    //bzero(buffer, 256);
    //fgets(buffer, 255, stdin);
    buffer = "Hi!";

    n = sendto(sockfd, &buffer[0], 256, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    } 

    //bzero(buffer, 256);
	
    struct sockaddr_in serv2;
    unsigned int serv2_size = sizeof(serv2);    


    n = recvfrom(sockfd, &buffer[0], 255, 0, (struct sockaddr *) &serv2, &serv2_size);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("%s\n", buffer);

    close(sockfd);

    return 0;
}*/

