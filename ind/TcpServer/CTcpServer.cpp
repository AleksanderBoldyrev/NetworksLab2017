#include "stdinc.h"
#include "CTcpServer.h"

CTcpServer::CTcpServer() {
    AcceptSock = 0;
    LastClientID = 0;
    pthread_mutex_init(&Mut, 0);
}

CTcpServer::CTcpServer(const CTcpServer& orig) {
}

CTcpServer::~CTcpServer() {
    pthread_mutex_destroy(&Mut);
}

void CTcpServer::StartAccept(USHORT Port)
{
    LockMutex(&Mut);

    AcceptThInfo.ThPort = Port;
    AcceptThInfo.bCreated = true;

    AcceptThInput* ThInput = new AcceptThInput;
    ThInput->ThPort = Port;
    ThInput->pAcceptSock = &AcceptSock;
    ThInput->pParent = this;

    pthread_create(&AcceptThInfo.ThHandle, 0, AcceptThread, (void *) ThInput);

    UnlockMutex(&Mut);
}

void CTcpServer::StartListenTh(SOCKET Sock)
{
    LockMutex(&Mut);
    ClientInfo CliInfo;
    CliInfo.ID = LastClientID++;
    CliInfo.ClientSocket = Sock;

    ListenThInput* pThInput = new ListenThInput;
    pThInput->CliID = CliInfo.ID;
    pThInput->ClientSocket = Sock;
    pThInput->pParent = this;

    pthread_create(&(CliInfo.ClientThreadInfo.ThHandle), NULL, ListenThread, pThInput);

    UnlockMutex(&Mut);
}

void* AcceptThread(void* pParam)
{
    AcceptThInput* pOldData = (AcceptThInput*) pParam;
    AcceptThInput pData = (*pOldData);
    delete pOldData;
    (*pData.pAcceptSock) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ((*pData.pAcceptSock) == -1) {
        printf("Socket create failed\r\n");
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(pData.ThPort);

    int res;
    res = bind((*pData.pAcceptSock), (sockaddr *) & service, sizeof (service));
    if (res!=0)
    {
        printf("Bind failed\r\nFailed to bind %d port.", pData.ThPort);
        pData.ThPort += 1;
        service.sin_port = htons(pData.ThPort);
        bind((*pData.pAcceptSock), (sockaddr *) & service, sizeof (service));
    }
    printf("MAIL SERVER\nPort:%d\r\n", (int) pData.ThPort);
    if (listen((*pData.pAcceptSock), SOMAXCONN) == -1) {
        printf("Listen failed\r\n");
    }

    SOCKET AcceptSocket;
    bool bQuit = false;
    while (!bQuit) {
        AcceptSocket = accept((*pData.pAcceptSock), 0, 0);
        if (AcceptSocket == -1) {
            printf("Accept closed\r\n");
            bQuit = true;
        } else {
            printf("New client accepted\r\n");
            pData.pParent->StartListenTh(AcceptSocket);
        };
    }
    return 0;
};

void* ListenThread(void* pParam) {
    printf("Listen thread is run\r\n");
    ListenThInput* pOldData = (ListenThInput*) pParam;
    ListenThInput pData = (*pOldData);
    delete pOldData;

    ServerWorker w;
    w.init(pData.ClientSocket);
    bool res = w.mainLoop();
    if (res)
        printf("Client #%d terminated successfully!\n", pData.CliID);
    else
        printf("Client #%d terminated abnormally!\n", pData.CliID);
    pthread_exit(0);
    
};

void LockMutex(pthread_mutex_t* mut) {
#ifdef PRINT_DEADLOCK
    bool bDeadlock = false;
    if (mut->__data.__lock != 0) {
        printf("--TCPIO DEADLOCKED\r\n");
        bDeadlock = true;
    }
#endif
    pthread_mutex_lock(mut);
#ifdef PRINT_DEADLOCK
    if (bDeadlock) {
        printf("--TCPIO UNLOCKED\r\n");
    }
#endif
}

void UnlockMutex(pthread_mutex_t* mut) {
    pthread_mutex_unlock(mut);
}