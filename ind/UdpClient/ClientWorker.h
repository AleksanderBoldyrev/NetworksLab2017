#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <stdlib.h>
//#include <winsock2.h>
//#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
//#include <conio.h>
#include <string>
#include <sstream> 
#include <algorithm>

#pragma "ws2_32.lib"
#pragma "lpthread.lib"

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
//#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <map>
#include <signal.h>
#include <assert.h>
#include <string>
#include <sstream> 
#include <string.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/file.h>

#include "API.h"

const char DELIM = ':';

static bool dr = false;
static string uname;
static int mesId = -1;

using namespace std;

class ClientWorker
{
public:
	ClientWorker();
	~ClientWorker();

	void StartThread(string* params);
	void StopThread();
	static void* HandleThread(void* args);
	void SendTo(int s, const string& message);
	bool ListenRecv(int s, std::string& MsgStr);
	string Serialize(unsigned int opcode, unsigned short numarg, const string* ss);
	STATE Parse(const string& input, unsigned short& numarg, string* &args);
	string MessageToString(const Message& m);
        void ProcessRes(short& state, const string& buf, Message& m, const string& mes);
private:
	void Run(string host, unsigned short port);
	void ListenLoop(string host, unsigned short port);
	STATE ParseOpCode(const string& buf);
	pthread_t tHandle;
        
        struct sockaddr_in servOut;
        struct hostent *server;
        struct sockaddr_in servIn;
        unsigned int servIn_size = sizeof(servIn);
        int sockfd;
        
	unsigned long lastPacketNumSend = 0;
	unsigned long lastPacketNumRecv = 0;
        
	string tempRBuf;
};
