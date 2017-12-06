#pragma once

#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <conio.h>
#include <string>
#include <sstream> 
#include <algorithm>

#include "API.h"

#define DELIM ':'

using namespace std;

class ClientWorker
{
public:
	ClientWorker();
	~ClientWorker();

	void startThread(string* params);
	void stopThread();
	static DWORD WINAPI runner(LPVOID args);
	void sendTo(SOCKET s, const string& message);
	bool ListenRecv(SOCKET s, std::string& MsgStr);
	string serialize(unsigned int opcode, unsigned short numarg, const string* ss);
	//STATE ClientWorker::parse(const string& input, unsigned short& numarg, string* args);
	STATE parse(const string& input, unsigned short& numarg, string* &args);
	string MessageToString(const Message& m);
private:
	void run(string host, unsigned short port);
	int readN(SOCKET s, char* buf, int remain, int flags);
	void ListenLoop(const int& socket);
	//STATE ClientWorker::parseOpCode(const string& buf);
	STATE parseOpCode(const string& buf);
	HANDLE tHandle;
};
