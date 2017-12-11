#pragma once
#include <string>
#include <algorithm>
#pragma "ws2_32.lib"
#pragma "lpthread.lib"

using namespace std;

const char DELIM_PARSE =  '|';
const char DELIM_SERIALIZE =  '^';
const char EOF_SYM = (char)2;

const short UDP_DG_LEN = 32;     // UNICODE symbol count in one packet.
const short TECH_DG_SIZE = 10;     // Char count, in which the length of the packet and it's number are secured.

const string OPENT = "Enter your option: ";

static const int API_SIZE = 18;

static const string API [API_SIZE] = 
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

const short MESSAGE_FIELDS_COUNT = 6;

const string MES_ID = "<id>";
const string MES_ADDR = "<from>";
const string MES_DATE_TIME = "<date/time>";
const string MES_LEN = "<len>";
const string MES_STATE = "<state>";             // message read/unread/deleted

static const string MESSAGE_STATES[3] = { "Normal", "Deleted", "Unread" };

const short MSTATE_NORMAL = 0;     // position of normal value
const short MSTATE_DELETED = 1;    // position of deleted value
const short MSTATE_UNREAD = 2;     // position of unread value

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
	int state = MSTATE_NORMAL;
	string body;
	string Serialize();
	void Clear();
	bool Deserialize(const string& input);
};
