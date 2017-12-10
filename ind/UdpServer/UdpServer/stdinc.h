#pragma once
#include <direct.h> 
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <assert.h>
#include <sstream> 
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdint.h>
#include <locale>
#include <codecvt>
#include <fcntl.h>

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

	~ThreadData()
	{
		if (rBuf != nullptr)
			delete rBuf;
		if (sBuf != nullptr)
			delete sBuf;
	}
};

