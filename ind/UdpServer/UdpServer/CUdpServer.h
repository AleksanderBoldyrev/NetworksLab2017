#pragma once
#pragma once

#include "stdinc.h"

#include "ServerWorker.h"

typedef unsigned short  USHORT;
//typedef int             SOCKET;
typedef int             ClientID;

#define PRINT_DEADLOCK

#define TIMEOUT_MS			100     /* Receive timeout. Seconds between retransmits */
#define MUTEX_TIMEOUT		100
#define LISTEN_THREAD_SLEEP 10

using namespace std;

class CUdpServer {
public:
	CUdpServer();
	CUdpServer(const CUdpServer& orig);
	virtual ~CUdpServer();
	void StartAccept(USHORT Port);
	//void StartListenTh(SOCKET Sock);
private:
	//ThreadInfo AcceptThInfo;
	//ClientID LastClientID;
	ThreadData* clients;
	//SOCKET AcceptSock;
	HANDLE Mut;
	HANDLE serverThread;
	static bool LockMutex(HANDLE& m);
	static void UnlockMutex(HANDLE& m);
	static bool LockMutex(const wstring& name, HANDLE& m);
	unsigned int clientsSize = 0;
	static DWORD WINAPI AcceptThread(LPVOID pParam);
	static DWORD WINAPI ListenThread(LPVOID pParam);
	static void processInput(const SOCKET& s, ThreadData* &clients, unsigned int& cSize);
	static void processOutput(const SOCKET& s, ThreadData* clients, const unsigned int& cSize);
	static unsigned int checkClient(const sockaddr_in& saddr, ThreadData* &clients, unsigned int& cSize);
	//static string intToStr(const unsigned int& val);
};