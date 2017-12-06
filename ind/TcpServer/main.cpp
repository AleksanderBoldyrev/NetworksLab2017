#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "CTcpServer.h"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "en_US.UTF-8");
    char str[512];
    CTcpServer Server;
    Server.StartAccept(5555);
    while(true){
        scanf("%s",str);
		if(strcmp(str,"exit")==0){ 
                    //Server.DisconnectAll();
                    //return 0;
                    exit(0);
                    break;
                }
    }
    return (EXIT_SUCCESS);
}

