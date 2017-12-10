#pragma once
#pragma once

#include "stdinc.h"

#include "ServerWorker.h"

typedef unsigned short  USHORT;
typedef int             ClientID;

#define PRINT_DEADLOCK

#define TIMEOUT_MS			1000     /* Receive timeout. Seconds between retransmits */
#define MUTEX_TIMEOUT		100
#define LISTEN_THREAD_SLEEP 10

using namespace std;

class CUdpServer {
public:
	CUdpServer();
	CUdpServer(const CUdpServer& orig);
	virtual ~CUdpServer();
	void StartAccept(USHORT Port);
private:
	ThreadData* clients;
	HANDLE Mut;
	HANDLE ServerThread;
	static bool LockMutex(HANDLE& m);
	static void UnlockMutex(HANDLE& m);
	static bool LockMutex(const wstring& name, HANDLE& m);
	unsigned int clientsSize = 0;
	static DWORD WINAPI AcceptThread(LPVOID pParam);
	static DWORD WINAPI ListenThread(LPVOID pParam);
	static void ProcessInput(const SOCKET& s, ThreadData* &clients, unsigned int& cSize);
	static void ProcessOutput(const SOCKET& s, ThreadData* clients, const unsigned int& cSize);
	static unsigned int CheckClient(const sockaddr_in& saddr, ThreadData* &clients, unsigned int& cSize);
};