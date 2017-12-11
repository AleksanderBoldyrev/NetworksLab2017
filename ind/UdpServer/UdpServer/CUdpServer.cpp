#include "stdinc.h"
#include "CUdpServer.h"

CUdpServer::CUdpServer() {

}

CUdpServer::CUdpServer(const CUdpServer& orig) {
}

CUdpServer::~CUdpServer() {
	CloseHandle(Mut);
}

DWORD WINAPI CUdpServer::ListenThread(LPVOID pParam) {
	printf("UDP thread is run\r\n");
	ThreadData* l = (ThreadData*)pParam;
	if (l != nullptr)
	{
		ServerWorker w;
		w.Init(l);
		bool res = w.MainLoop();
		if (res)
			printf("Client %s terminated successfully!\n", inet_ntoa(l->address.sin_addr));
		else
			printf("Client %s terminated abnormally!\n", inet_ntoa(l->address.sin_addr));
	}
	else
	{
		cout << "Received null data structure!" << endl;
		return -1;
	}
	return 0;
};


DWORD WINAPI CUdpServer::AcceptThread(LPVOID pParam)
{
	if (pParam != nullptr)
	{
		WSADATA wsaData;
		unsigned short port = *((USHORT*)pParam);
		unsigned int n;
		n = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (n != 0) {
			printf("WSAStartup failed: %ui\n", n);
			return 1;
		}
		SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s == -1) {
			printf("Socket create failed\r\n");
			return -1;
		}

		ThreadData* clients = nullptr;
		unsigned int cSize = 0;

		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = INADDR_ANY;
		service.sin_port = htons(port);

		int res;
		static int timeout = TIMEOUT_MS;
		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		res = bind(s, (sockaddr *)& service, sizeof(service));
		if (res != 0)
		{
			printf("Bind failed\r\nFailed to bind %d port.", port);
			port++;
			service.sin_port = htons(port);
			bind(s, (sockaddr *)& service, sizeof(service));
		}
		printf("MAIL SERVER\nPort:%d\r\n", (int)port);

		while (1)
		{
			ProcessInput(s, clients, cSize);
			ProcessOutput(s, clients, cSize);

			Sleep(LISTEN_THREAD_SLEEP);
		}
		closesocket(s);

		return 0;
	}
	return -1;
};


//If client has current IP, then returns client ID, else it creates a new thread, puts new client into DB and returns new ID. 
unsigned int CUdpServer::CheckClient(const sockaddr_in& saddr, ThreadData* &clients, unsigned int& cSize)
{
	if (cSize > 0)
	{
		for (int i = 0; i < cSize; i++)
		{
			DWORD result = WaitForSingleObject(clients[i].tHandle, 10);

			if (result == WAIT_OBJECT_0) {
				// the thread handle is signaled - the thread has terminated
				int ind = 0;
				cSize--;
				ThreadData * arr = new ThreadData[cSize];
				if (cSize > 1)
					for (unsigned int j = 0; j < cSize + 1; j++)
					{
						if (j != i)
						{
							ind++;
							arr[ind] = clients[j];
						}
					}
				delete[] clients;
				clients = arr;
			}
		}

		for (int i = 0; i < cSize; i++)
		{
			if ((saddr.sin_addr.S_un.S_addr == clients[i].address.sin_addr.S_un.S_addr) && (saddr.sin_port == clients[i].address.sin_port))
			{
				return i; // we found our client
			}
		}

		// let's create a new client thread
		cSize++;
		ThreadData * arr = new ThreadData[cSize];
		if (cSize > 1)
			for (unsigned int i = 0; i < cSize - 1; i++)
				arr[i] = clients[i];
		delete[] clients;
		clients = arr;
	}
	else
	{
		cSize++;
		clients = new ThreadData[cSize];
	}

	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

	clients[cSize - 1].address = saddr;

	clients[cSize - 1].rBuf = nullptr;

	stringstream ss;
	ss << inet_ntoa(saddr.sin_addr) << "_" << saddr.sin_port << "_read";
	clients[cSize - 1].rMutexName = converter.from_bytes(ss.str().c_str());
	ss.str(string());
	ss << inet_ntoa(saddr.sin_addr) << "_" << saddr.sin_port << "_write";
	clients[cSize - 1].sMutexName = converter.from_bytes(ss.str().c_str());

	clients[cSize - 1].sBuf = nullptr;
	clients[cSize - 1].rBuf = nullptr;

	DWORD t;
	clients[cSize - 1].tHandle = CreateThread(0, 0, ListenThread, (void*)&clients[cSize - 1], 0, &t);
	clients[cSize - 1].tId = t;

	return cSize-1;
}

void CUdpServer::ProcessInput(const SOCKET& s, ThreadData* &clients, unsigned int& cSize)
{
	sockaddr_in from;
	int fromlen = sizeof(from);
	char buffer[UDP_DG_LEN];
	string buff2;
	ZeroMemory(buffer, sizeof(buffer));
	while (recvfrom(s, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
	{
		HANDLE mutex;
		// write data into read buffer of clients[pos]
		buff2.clear();
		for (int i = 0; i < UDP_DG_LEN; i++)
		{
			buff2 += buffer[i];
			if (buffer[i] == EOF_SYM)
				break;
		}
		ZeroMemory(buffer, sizeof(buffer));
		if (buff2.length() > 0)
		{
			buff2.erase(std::remove(buff2.begin(), buff2.end(), '\r'), buff2.end());
			buff2.erase(std::remove(buff2.begin(), buff2.end(), '\0'), buff2.end());
			cout << "Recieved: " << buff2 << endl;

			unsigned int pos = CheckClient(from, clients, cSize);
			//
			string str = buff2;
			size_t num = 0;
			if (buff2.length() > TECH_DG_SIZE)
			{
				string c = str.substr(0, TECH_DG_SIZE);
				str = str.substr(TECH_DG_SIZE, str.length() - TECH_DG_SIZE);
				num = atoi(c.c_str());
			}
			if (clients[pos].lastPacketNumRecv + 1 == num)
			{
				clients[pos].lastPacketNumRecv = num;
				for (int i = 0; i < str.length(); i++)
				{
					if (str[i] == EOF_SYM)
					{
						bool act = false;
						while (!act)
						{
							if (LockMutex(clients[pos].rMutexName, mutex))
							{
								// read data
								if (clients[pos].rBuf != nullptr)
									strcat(clients[pos].rBuf, clients[pos].tempRBuf);
								clients[pos].address = from;
								act = true;
								clients[pos].tempRBuf = "";
								// unlock read mutex of clients[pos]
							}
							UnlockMutex(mutex);
						}
					}
					else
						clients[pos].tempRBuf += str[i];
				}
			}
			else
				cout << "Packet num mismatch!\n";
		}
	}
}

void CUdpServer::ProcessOutput(const SOCKET& s, ThreadData* clients, const unsigned int& cSize)
{
	if (cSize > 0 && clients != nullptr)
	{
		// fill send flags for each client
		bool* flags = new bool[cSize];
		int flagsSize = cSize;
		for (int i = 0; i < cSize; i++)
			flags[i] = false;

		for (int i = 0; i < cSize; i++)
		{
			//Lock write mutex for each client; 
			HANDLE m;
			bool act = false;
			int count = 0;
			while (!act && count<16)
			{
				count++;
				if (!LockMutex(clients[i].sMutexName, m))
				{
					UnlockMutex(m);
				}
				else
				{
					//read data from sBuf;
					if (clients[i].sBuf != nullptr)
					{
						char* ss = clients[i].sBuf;
						
						unsigned int curPos = 0;
						// Разбиваем ss на подстроки длины количества символов UDP пакета и каждый раз отправляем их как новые пакеты.

						if (strlen(ss) > 0)
						{
							ss += EOF_SYM;
							delete[] clients[i].sBuf;
							int tolen = sizeof(clients[i].address);
							char* sndBuf;
							const int tlen = strlen(ss);
							while (curPos < tlen)
							{
								delete[] sndBuf;
								int num = ++clients[i].lastPacketNumSend;
								itoa(num, sndBuf, 10);
								int tLen = min(tlen -curPos, UDP_DG_LEN - TECH_DG_SIZE);
								char* subbuff = new char[tlen];
								memcpy(subbuff, &ss, tlen - 1);
								curPos += tLen;
								cout << "Sending to client: " << sndBuf << endl;
								int count = sendto(s, sndBuf, strlen(sndBuf), 0, (sockaddr*)&clients[i].address, tolen);
								if (count != strlen(sndBuf))
									cout << "Send data mismatch: send " << count << ", have " << strlen(sndBuf) << endl;
								act = true;
							}
						}
					}
					else
						cout << "Send buffer is nullptr for client #" << i << endl;

					//unlock mutex;
					UnlockMutex(m);
				}
			}
		}
	}
}

bool CUdpServer::LockMutex(HANDLE& m)
{
	m = CreateMutex(NULL, FALSE, L"UdpServer");
	DWORD result;
	result = WaitForSingleObject(m, MUTEX_TIMEOUT);
	if (result != WAIT_OBJECT_0)
	{
		return false;
	}
	return true;
}

void CUdpServer::UnlockMutex(HANDLE& m)
{
	CloseHandle(m);
}

bool CUdpServer::LockMutex(const wstring& name, HANDLE& m)
{
	m = CreateMutex(NULL, FALSE, name.c_str());
	DWORD result;
	result = WaitForSingleObject(m, MUTEX_TIMEOUT);
	if (result != WAIT_OBJECT_0)
	{
		return false;
	}
	return true;
}

void CUdpServer::StartAccept(USHORT port)
{
	if (LockMutex(Mut))
	{
		DWORD t;
		USHORT* pp = new USHORT;
		*pp = port;
		ServerThread = CreateThread(0, 0, AcceptThread, (void *)pp, 0, &t);
		UnlockMutex(Mut);
	}
}