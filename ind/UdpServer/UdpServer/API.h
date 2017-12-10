#pragma once
#ifndef API_H
#define API_H

#include <string>
#include <algorithm>

using namespace std;

#define DELIM_PARSE '|'
#define DELIM_SERIALIZE '^'
#define EOF_SYM (char)2

#define UDP_DG_LEN      32     // UNICODE symbol count in one packet.
#define TECH_DG_SIZE    10      // Char count, in which the length of the packet and it's number are secured.

const short STRING_BUFFER_SIZE = 1024;
const short BUFSIZE = 10;

static const int API_SIZE = 18;

static const string API[API_SIZE] =
{
	"Yes",
	"Noop",
	"Answer",
	"Start",
	"Init",
	"Operation Code",
	"Exit",
	"Register",
	"Login",
	"Logout",
	"Send mes",
	"Delete user",
	"Delete mes",
	"Show unread",
	"Show all",
	"Show exact",
	"Resend",
	"In system"
};

enum STATE
{
	SERV_OK,
	NO_OPERATION,
	ANSWER,
	START,
	INIT,
	OPCODE,
	EXIT,
	REG,
	LOG,
	LOGOUT,
	SND,
	DEL_US,
	DEL_MSG,
	SH_UNR,
	SH_ALL,
	SH_EX,
	RSND,
	INSYS
};

#define MESSAGE_FIELDS_COUNT 6

#define MES_ID          "<id>"
#define MES_ADDR        "<from>"
#define MES_DATE_TIME   "<date/time>"
#define MES_LEN         "<len>"
#define MES_STATE       "<state>"             // message read/unread/deleted

const int MESSAGE_STATES[] = { 0, 1, 2 };

#define MSTATE_NORMAL 0     // position of normal value
#define MSTATE_DELETED 1    // position of deleted value
#define MSTATE_UNREAD 2     // position of unread value

static string intToStr(const unsigned int& val)
{
	stringstream sss;
	sss << val;
	string sk = sss.str();
	while (sk.size() < TECH_DG_SIZE)
	{
		sk.insert(sk.begin(), '0');
	}
	return sk;
}

class Message
{
public:
	unsigned long id = 0;
	string username;
	string date_time;
	unsigned long len = 0;
	int state = 0;
	string body;
	string Serialize();
	bool Deserialize(const string& input);
};

#endif