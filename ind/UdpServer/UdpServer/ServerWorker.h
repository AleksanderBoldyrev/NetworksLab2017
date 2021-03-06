#pragma once
/*
* To change this license header, choose License Headers in Project Properties.
* To change this template file, choose Tools | Templates
* and open the template in the editor.
*/

/*
* File:   SeverWorker.h
* Author: root
*
* Created on October 22, 2017, 3:13 AM
*/
#pragma once

#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include "stdinc.h"
#include "API.h"

#define SEMAP_TIMEOUT		100

#define USERS_FOLDER "./users/"
#define MESSAGE_FILE "/messages"
#define PASSW_FILE   "/password"

typedef unsigned short  USHORT;

using namespace std;

class ServerWorker
{
public:
	ServerWorker();
	~ServerWorker();

	void Init(ThreadData* data);
	bool MainLoop();
private:
	void SendTo(const char* message);
	ThreadData* td;
	string GetPasswFilePth(const string& username);
	string GetMessageFilePth(const string& username);
	string LoginNewUser(const string &uname, const string &passw, bool &res);
	string RegisterNewUser(const string &uname, const string &passw, bool &res);
	string DeleteUser(const string& username);
	unsigned long AddMessage(Message* message, const string& username, const string& from, string& err);
	string ShowUnreadMes(const string& username, unsigned long &cc, string* &buf);
	string ShowAllMes(const string& username, unsigned long &size, string* &buf);
	string ShowExactMes(const string& username, string& buf, const string& mesNumber);
	string DeleteMes(const string& username, const string& mesNumber);
	string ResendMes(const string& from, const string& to, string& buf, const string& mesNumber);
	string MessageToString(const Message& m);
	Message** ReadAllMes(const string& username, unsigned long& size);
	Message* ReadOneMes(const string& username, const unsigned long& id, bool& res);
	bool DeleteOneMes(const string& username, const unsigned long& id);
	bool WriteMessages(const string& username, Message** m, const unsigned long& size, bool ioMode);//const Message** m, const unsigned long& size);
	bool CheckUser(const string& name);
	unsigned long LastMesID(const string& username);
	STATE ParseOpCode(const string& buf);
	string Serialize(STATE opcode, unsigned short numarg, const string * ss);
	STATE Parse(const string& input, unsigned short& numarg, string* &args);

	void WriteToFile(const string& username, Message* message);

	void OpenSem(const string& name);
	void CloseSem(const string& name);

	bool ListenInBuf(char* &MsgStr);

	static bool LockMutex(HANDLE& m);
	static void UnlockMutex(HANDLE& m);
	static bool LockMutex(const wstring& name, HANDLE& m);
	HANDLE sem; // ��������� ��������
};


#endif /* SEVERWORKER_H */

