#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
//#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h> 

void sendAndReceive(int socket)
{
	srand(time(0));
	char buffer[256];
	unsigned int n;
	while (1)
	{
		//printf("Please enter the message: ");
		memset(buffer, 0, 256);
		//fgets(buffer, 255, stdin);

		int hours, minutes, seconds, day, month, year;
		time_t now;
		time(&now);
		struct tm *local = localtime(&now);
		hours = local->tm_hour;			// get hours since midnight	(0-23)
		minutes = local->tm_min;		// get minutes passed after the hour (0-59)
		seconds = local->tm_sec;		// get seconds passed after the minute (0-59)
		day = local->tm_mday;			// get day of month (1 to 31)
		month = local->tm_mon + 1;		// get month of year (0 to 11)
		year = local->tm_year + 1900;	// get year since 1900
		sprintf(buffer, "Time is : %02d:%02d:%02d\n", hours, minutes, seconds);

		n = strlen(buffer);
		/* Send message to the server */
		send(socket, buffer, strlen(buffer), 0);

		/*if (n < 0) {
			perror("ERROR writing to socket");
			exit(1);
		}*/

		/* Now read server response */
		memset(buffer, 0, 256);
		// n = recv(sockfd, buffer, 255, 0);
		n = recv(socket, buffer, 20 + n, 0);

		/*if (n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}*/

		printf("%s\n", buffer);

		Sleep(rand() % 3000);
	}
}

int main(int argc, char *argv[]) {
    unsigned int n;
    WSADATA wsaData;
    n = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (n != 0) {
        printf("WSAStartup failed: %d\n", n);
        return 1;
    }
    
    int sockfd;
    uint16_t portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

   

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = (uint16_t) atoi(argv[2]);

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    memset((char*) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

	sendAndReceive(sockfd);

    /* Closing socket */
    n = shutdown(sockfd, SD_BOTH);
    if (n == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    closesocket(sockfd);
    WSACleanup();
    
    return 0;
}