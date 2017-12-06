#pragma once

//#include <pthread.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <unistd.h>
#include <direct.h> 
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
//#include <cstdlib>
#include <stdio.h>
#include <math.h>
//#include <vector>
//#include <list>
//#include <map>
#include <signal.h>
#include <assert.h>
#include <sstream> 
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdint.h>
#include <locale>
#include <codecvt>
//#include <sys/stat.h>
//#include <semaphore.h>
#include <fcntl.h>
//#include <sys/file.h>
//#include <sys/types.h>

class ThreadData
{
public:
	std::string* rBuf = nullptr;
	std::string* sBuf = nullptr;
	std::wstring sMutexName;
	std::wstring rMutexName;
	sockaddr_in address;
	HANDLE tHandle;
	DWORD tId;

	unsigned long lastPacketNumSend = 0;
	unsigned long lastPacketNumRecv = 0;
	std::string tempRBuf;
	//unsigned long mesRLen = 0;

	~ThreadData()
	{
		if (rBuf != nullptr)
			delete rBuf;
		if (sBuf != nullptr)
			delete sBuf;
	}
};

