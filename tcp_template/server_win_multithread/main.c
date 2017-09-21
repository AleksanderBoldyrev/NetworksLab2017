#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdint.h>
#include <string.h>
#include "pthread.h"

char* foo = "I got your message: ";
const static unsigned int n = 20;

int lastId = 0;

void* readAndTranslate(void * param)
{
	int id = lastId++;
	int flags = 0;
	int s = *((int*)param);
	char buffer[256];
	char backBuffer[1024];
	while (1)
	{
		memset(buffer, 0, 256);
		int rc = recv(s, buffer, 255, flags);
		if (rc > 0)
		{
			printf("THREAD #%i: %s\n", id, buffer);
			int len = strlen(buffer);
			memset(backBuffer, 0, n + strlen(buffer));
			//printf("size = %i\n",len);
			for (unsigned int i = 0; i < n; i++)
				backBuffer[i] = foo[i];
			for (unsigned int i = n; i < 255 + n; i++)
				backBuffer[i] = buffer[i-n];
			//printf("THREAD #%i: %s\n", id, backBuffer);
			send(s, backBuffer, n +len, 0);
		}
		if (WSAGetLastError() != 0)
			pthread_exit(0);
	}
}

int main(int argc, char *argv[]) {
	
    WSADATA wsaData;
	
    unsigned int n;
    n = WSAStartup(MAKEWORD(2,2), &wsaData);
	
    if (n != 0) {
        printf("WSAStartup failed: %ui\n", n);
        return 1;
    }
    
    int sockfd, newsockfd;
    uint16_t portno;
    int clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    portno = 5001;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here process will
       * go in sleep mode and will wait for the incoming connection
    */

	while (1)
	{
		listen(sockfd, 5);
		clilen = sizeof(cli_addr);

		/* Accept actual connection from the client */
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}

		if (WSAGetLastError() != 0)
		{
			exit(1);
		}

		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, readAndTranslate, &newsockfd);
		pthread_detach(tid, NULL);
	}
    /* If connection is established then start communicating */
    //memset(buffer, 0, 256);
    //n = recv(newsockfd, buffer, 255, 0); // recv on Windows
	//n = readN(newsockfd, buffer, 255, 0);

    /*if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }*/

   // printf("Here is the message: %s\n", buffer);

    /* Write a response to the client */
    // n = send(newsockfd, "I got your message", 18, 0); // send on Windows
     /*if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }*/
    
    /* Closing socket */
    shutdown(sockfd, SD_BOTH);
    /*if (n == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }*/
    closesocket(sockfd);
    WSACleanup();

    return 0;
}