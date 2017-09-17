#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

int readN(int sfd, char* const data, size_t* size)
{
    ssize_t n;
    size_t total = 0;
    size_t bytesleft = *size;

    while(total < *size)
    {
        n = recv(sfd, data + total, bytesleft, MSG_NOSIGNAL);
        if(-1 == n || 0 == n)
        {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    *size = total;
    data[total] = '\0';

    return -1 == n ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*void readN(size_t size, char* buffer, int sockId)
{
    size_t curPos = 0;
    ssize_t n;
    size_t tmpBufSize = 255;
    char tmp[tmpBufSize];

    while (curPos<size)
    {
        if (size - curPos < tmpBufSize)
            tmpBufSize = size - curPos;
        bzero(tmp, size);
        n = read(sockId, tmp, tmpBufSize); // recv on Windows
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }
        for (size_t i = 0; i<tmpBufSize; i++)
            buffer[i+curPos] = tmp[i];
        curPos = curPos + tmpBufSize;
    }
} */

int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    uint16_t portno;
    unsigned int clilen;
    char buffer[8192];
    struct sockaddr_in serv_addr, cli_addr;
    ssize_t n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
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

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    /* Accept actual connection from the client */
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    /* If connection is established then start communicating */
    //readN(8192, buffer, newsockfd);
    size_t limit = 255;
    if(0 != readn(newsockfd, buffer, &limit))
    {
        fprintf(stderr, "%ld bytes were read, but recv() call failed:\n", limit);
        perror("");
        exit(1);
    }
    shutdown(newsockfd, SHUT_RD);

    printf("Here is the message: %s\n", buffer);

    /* Write a response to the client */
    n = write(newsockfd, "I got your message", 18); // send on Windows
    shutdown(newsockfd, SHUT_WR);

    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    shutdown(sockfd, 2);
    close(sockfd);

    return 0;
}
