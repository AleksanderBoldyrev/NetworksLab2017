#pragma once
#include <string>
#include <algorithm>

using namespace std;

#define DELIM_PARSE '|'
#define DELIM_SERIALIZE '^'

#define OPENT "Enter your option: "

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
	DEL_MES,
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

static const string MESSAGE_STATES[3] = { "Normal", "Deleted", "Unread" };

#define MSTATE_NORMAL 0     // position of normal value
#define MSTATE_DELETED 1    // position of deleted value
#define MSTATE_UNREAD 2     // position of unread value

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