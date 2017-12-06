#pragma once

#include "stdinc.h"
#include <iostream>
#include <fstream>
#include <string>
#include "time.h"
#include "ServerWorker.h"

typedef unsigned short  USHORT;
typedef int             SOCKET;
typedef int             ClientID;

#define PRINT_DEADLOCK

using namespace std;
        
class CTcpServer;
void* AcceptThread(void* pParam);
void* ListenThread(void* pParam);

void CloseThread(pthread_t ThH,bool bTerminate);
void LockMutex(pthread_mutex_t* mut);
void UnlockMutex(pthread_mutex_t* mut);

struct AcceptThInput
{
    AcceptThInput()
    {
        ThPort=0;
        pParent=0;
        pAcceptSock=0;
    };

    USHORT ThPort;
    SOCKET* pAcceptSock;
    CTcpServer* pParent;
};

struct ListenThInput
{
    ListenThInput()
    {
        ClientSocket=0;
        pParent=0;
        CliID=0;
        State=1;
        CurQst=1;
        result=0;
    };

    SOCKET ClientSocket;
    ClientID CliID;
    CTcpServer* pParent;
    int State;
    int CurQst;
    int result;
};

struct ThreadInfo
{
    ThreadInfo()
    {
        ThHandle=0;
        ThPort=0;
        bCreated=false;
    };

    pthread_t ThHandle;
    USHORT ThPort;
    bool bCreated;
};

struct ClientInfo
{
    ClientInfo()
    {
        ID=0;
        ClientSocket=0;
    };

    ClientID ID;
    SOCKET ClientSocket;
    ThreadInfo ClientThreadInfo;
};

bool ListenProc(ListenThInput& pData,int& State,int& CurQst,int& result);
bool ListenRecv(ClientID& From, std::string& MsgStr,ListenThInput& pData);
bool register_user(char* user_name,CTcpServer* pData,ClientID id);

typedef std::map<ClientID,ClientInfo> ClientsMapArray;
typedef std::map<ClientID,ClientInfo>::iterator ClientsMapArrayIt;
typedef std::pair<ClientID,ClientInfo> ClientsPair;

class CTcpServer {
public:
    CTcpServer();
    CTcpServer(const CTcpServer& orig);
    virtual ~CTcpServer();
    void StartAccept(USHORT Port);
    void StartListenTh(SOCKET Sock);
private:
    ThreadInfo AcceptThInfo;
    ClientID LastClientID;
    SOCKET AcceptSock;
    pthread_mutex_t Mut;
};