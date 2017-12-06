#include "stdinc.h"
#include "CUdpServer.h"

CUdpServer::CUdpServer() {
	//AcceptSock = 0;
	//LastClientID = 0;
	//Mut = CreateMutex(NULL, FALSE, L"UdpServer");
}

CUdpServer::CUdpServer(const CUdpServer& orig) {
}

CUdpServer::~CUdpServer() {
	CloseHandle(Mut);
}

DWORD WINAPI CUdpServer::ListenThread(LPVOID pParam) {
	printf("UDP thread is run\r\n");
	/*ListenThInput* pOldData = (ListenThInput*)pParam;
	ListenThInput pData = (*pOldData);*/
	//delete pOldData;
	ThreadData* l = (ThreadData*)pParam;
	if (l != nullptr)
	{
		ServerWorker w;
		w.init(l);
		bool res = w.mainLoop();
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
	//pthread_exit(0);
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

		//AcceptThInput* pOldData = (AcceptThInput*)pParam;
		//AcceptThInput pData = (*pOldData);
		//delete pOldData;
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
			//printf("Waiting...\n");
			processInput(s, clients, cSize);
			processOutput(s, clients, cSize);

			/*if (recvfrom(s, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
			{
				string str = buffer;
				//printf("Received message from %s: %s\n", inet_ntoa(from.sin_addr), buffer);
				processInput(str);
				processOutput(s, str);
			}*/
			Sleep(LISTEN_THREAD_SLEEP);
		}
		closesocket(s);
		//if (listen((*pData.pAcceptSock), SOMAXCONN) == -1) {
		//	printf("Listen failed\r\n");
		//}
		return 0;
	}
	return -1;
};


//If client has current IP, then returns client ID, else it creates a new thread, puts new client into DB and returns new ID. 
unsigned int CUdpServer::checkClient(const sockaddr_in& saddr, ThreadData* &clients, unsigned int& cSize)
{
	//if (clients == nullptr)
	///{
	//	clients = new ThreadData()
	//}
	//if (clients != nullptr)
	//{
	
	if (cSize > 0)
	{
		for (int i = 0; i < cSize; i++)
		{
			//if (saddr.sin_addr.S_un.S_addr == clients[i].address.sin_addr.S_un.S_addr && saddr.sin_family == clients[i].address.sin_family && saddr.sin_port == clients[i].address.sin_port && saddr.sin_zero == clients[i].address.sin_zero)
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
			//if (saddr.sin_addr.S_un.S_addr == clients[i].address.sin_addr.S_un.S_addr && saddr.sin_family == clients[i].address.sin_family && saddr.sin_port == clients[i].address.sin_port && saddr.sin_zero == clients[i].address.sin_zero)
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

	clients[cSize - 1].rBuf = new string();

	stringstream ss;
	ss << inet_ntoa(saddr.sin_addr) << "_" << saddr.sin_port << "_read";
	clients[cSize - 1].rMutexName = converter.from_bytes(ss.str().c_str());
	ss.str(string());
	ss << inet_ntoa(saddr.sin_addr) << "_" << saddr.sin_port << "_write";
	clients[cSize - 1].sMutexName = converter.from_bytes(ss.str().c_str());

	clients[cSize - 1].sBuf = new string();
	clients[cSize - 1].rBuf = new string();

	// lock read mutex
	/*HANDLE m = CreateMutex(NULL, FALSE, clients[cSize - 1].rMutexName.c_str());
	DWORD result;
	result = WaitForSingleObject(m, INFINITE);
	if (result != WAIT_OBJECT_0)
	{
		cout << "Failed to lock mutex!" << endl;
		cSize--;
		return 0;
	}*/

	DWORD t;
	clients[cSize - 1].tHandle = CreateThread(0, 0, ListenThread, (void*)&clients[cSize - 1], 0, &t);
	clients[cSize - 1].tId = t;

	return cSize-1;
	//}
	//return 0;
}

void CUdpServer::processInput(const SOCKET& s, ThreadData* &clients, unsigned int& cSize)
{
	sockaddr_in from;
	int fromlen = sizeof(from);
	char buffer[UDP_DG_LEN];
	string buff2;
	//stringstream bug;
	ZeroMemory(buffer, sizeof(buffer));
	//WSAEnumNetworkEvents();
	while (recvfrom(s, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
	{
		HANDLE mutex;// = CreateMutex(NULL, FALSE, clients[pos].rMutexName.c_str());
		// write data into read buffer of clients[pos]
		buff2.clear();
		for (int i = 0; i<UDP_DG_LEN; i++)
		{
			if (buffer[i] != EOF_SYM)
				buff2 += buffer[i];
			else
				break;
		}
		ZeroMemory(buffer, sizeof(buffer));
		if (buff2.length() > 0)
		{

			//buff2.erase(std::remove(buff2.begin(), buff2.end(), '\t'), buff2.end());
			buff2.erase(std::remove(buff2.begin(), buff2.end(), '\r'), buff2.end());
			//buff2.erase(std::remove(buff2.begin(), buff2.end(), '\n'), buff2.end());
			buff2.erase(std::remove(buff2.begin(), buff2.end(), '\0'), buff2.end());
			//remove(buff2.begin(), buff2.end(), '\t');
			//remove(buff2.begin(), buff2.end(), '\r');
			//remove(buff2.begin(), buff2.end(), '\0');
			//remove(buff2.begin(), buff2.end(), '\n');
			cout << "Recieved: " << buff2 << endl;

			unsigned int pos = checkClient(from, clients, cSize);
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
				// Пришел ли первый пакет и запоминаем длину
				if (clients[pos].mesRLen == 0)
				{
					if (str.length() >= TECH_DG_SIZE)
					{
						string c = str.substr(0, TECH_DG_SIZE);
						str = str.substr(TECH_DG_SIZE, str.length() - TECH_DG_SIZE);
						clients[pos].mesRLen = atoi(c.c_str());
						clients[pos].tempRBuf = str;
					}
					else
						cout << "First packet size is incorrect!\n";
				}
				else
				{
					clients[pos].tempRBuf += str;	
				}
				// Все ли данные прочитали?
				if (clients[pos].tempRBuf.length() == clients[pos].mesRLen)
				{
					//
					// lock mutex for client
					bool act = false;
					while (!act)
					{
						if (LockMutex(clients[pos].rMutexName, mutex))
						{
							// read data
							if (clients[pos].rBuf != nullptr)
								clients[pos].rBuf->append(clients[pos].tempRBuf);
							clients[pos].address = from;
							act = true;
							clients[pos].tempRBuf = "";
							clients[pos].mesRLen = 0;
							// unlock read mutex of clients[pos]
						}
						UnlockMutex(mutex);
					}
				}

			}
			else
				cout << "Packet num mismatch!\n";
		}
	}
}

void CUdpServer::processOutput(const SOCKET& s, ThreadData* clients, const unsigned int& cSize)
{
	//sendto(s, str.c_str(), str.length(), 0, (sockaddr*)&from, fromlen);
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
			/*HANDLE m = CreateMutex(NULL, FALSE, clients[i].rMutexName.c_str());
			DWORD result;
			result = WaitForSingleObject(m, INFINITE);*/
			HANDLE m;
			bool act = false;
			int count = 0;
			while (!act && count<16)
			{
				count++;
				if (!LockMutex(clients[i].sMutexName, m))
				{
					//cout << "Failed to lock mutex!" << endl;
					UnlockMutex(m);
				}
				else
				{
					//read data from sBuf;
					if (clients[i].sBuf != nullptr)
					{
						string ss = *clients[i].sBuf;
						unsigned int curPos = 0;
						// Разбиваем ss на подстроки длины количества символов UDP пакета и каждый раз отправляем их как новые пакеты.

						if (ss.size() > 0)
						{
							clients[i].sBuf->clear();
							int tolen = sizeof(clients[i].address);
							stringstream sndBuf;
							while (curPos < ss.length())
							{
								sndBuf.str(string());
								int num = ++clients[i].lastPacketNumSend;
								int dataCount = 1;
								sndBuf << intToStr(num);
								if (curPos == 0)
								{
									sndBuf << intToStr(ss.length());
									dataCount++;
								}
								int tLen = min(ss.length()-curPos, UDP_DG_LEN - dataCount*TECH_DG_SIZE);
								//cout << "\nSendind substr starting with: " << curPos << "; Count of symbols is " << tLen << "; lastPos = " << curPos+tLen << endl;
								sndBuf << ss.substr(curPos, tLen);
								curPos += tLen;
								if (sndBuf.str().length() < UDP_DG_LEN)
									sndBuf << EOF_SYM;
								cout << "Sending to client: " << sndBuf.str().c_str() << endl;
								int count = sendto(s, sndBuf.str().c_str(), sndBuf.str().length(), 0, (sockaddr*)&clients[i].address, tolen);
								if (count != sndBuf.str().length())
									cout << "Send data mismatch: send " << count << ", have " << sndBuf.str().length() << endl;
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
		//cout << "Failed to lock mutex!" << endl;
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
		//cout << "Failed to lock mutex!" << endl;
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
		serverThread = CreateThread(0, 0, AcceptThread, (void *)pp, 0, &t);
		UnlockMutex(Mut);
	}
}

//void CUdpServer::StartListenTh(SOCKET Sock)
//{
	/*LockMutex(Mut);
	ClientInfo CliInfo;
	CliInfo.ID = LastClientID++;
	CliInfo.ClientSocket = Sock;

	ListenThInput* pThInput = new ListenThInput;
	pThInput->CliID = CliInfo.ID;
	pThInput->ClientSocket = Sock;
	pThInput->pParent = this;

	//pthread_create(&(CliInfo.ClientThreadInfo.ThHandle), NULL, ListenThread, pThInput);

	DWORD t;
	HANDLE tHandle = CreateThread(0, 0, ListenThread, pThInput, 0, &t);
	clientsSize++;
	HANDLE * arr = new HANDLE[clientsSize];
	if (clientsSize > 1)
		for (unsigned int i = 0; i < clientsSize - 1; i++)
			arr[i] = clients[i];
	clients = arr;
	clients[clientsSize - 1] = tHandle;
	UnlockMutex(Mut);*/
	//}
