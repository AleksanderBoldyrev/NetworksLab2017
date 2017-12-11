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

const char DELIM = ':';
const short BUFSIZE = 10;

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
	static DWORD WINAPI HandleThread(LPVOID args);
	void SendTo(SOCKET s, const string& message);
	bool ListenRecv(SOCKET s, std::string& MsgStr);
	string Serialize(STATE opcode, unsigned short numarg, const string* ss);
	STATE Parse(const string& input, unsigned short& numarg, string* &args);
	string MessageToString(const Message& m);
	void ProcessRes(short& state, const string& buf, Message& m, const string& mes);
private:
	void Run(string host, unsigned short port);
	void ListenLoop(const int& socket);
	STATE ParseOpCode(const string& buf);
	HANDLE tHandle;
};
