#pragma once
#include <string>
#include <algorithm>
#pragma "ws2_32.lib"
#pragma "lpthread.lib"

using namespace std;

#define DELIM_PARSE '|'
#define DELIM_SERIALIZE '^'
#define EOF_SYM (char)2

#define UDP_DG_LEN      32     // UNICODE symbol count in one packet.
#define TECH_DG_SIZE    10      // Char count, in which the length of the packet and it's number are secured.

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
	LUG,
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
	string serialize()
	{
		stringstream ss;
		ss << id << DELIM_SERIALIZE;
		ss << username << DELIM_SERIALIZE;
		ss << date_time << DELIM_SERIALIZE;
		ss << len << DELIM_SERIALIZE;
		ss << state << DELIM_SERIALIZE;
		std::replace(body.begin(), body.end(), DELIM_SERIALIZE, ' ');
		ss << body << DELIM_SERIALIZE;
		return ss.str();
	};
	void clear()
	{
		id = 0;
		username = "";
		date_time = "";
		len = 0;
		state = MSTATE_NORMAL;
		body = "";
	};
	bool deserialize(const string& input)
	{
		bool res = true;
		int numarg = 0;
		string* args = NULL;
		if (input.size() > 0)
		{
			stringstream buf;
			numarg = 0;
			// find all delimeters
			for (int i = 0; i < input.size(); i++)
			{
				if (input[i] == DELIM_SERIALIZE)
					numarg++;
			}
			// find all parts
			if (numarg > 0)
			{
				args = new string[numarg];
				unsigned short cc = 0;
				for (int i = 0; i < input.size(); i++)
				{
					if (input[i] == DELIM_SERIALIZE)
					{
						args[cc] = buf.str();
						cc++;
						buf.str(std::string());
					}
					else
					{
						buf << input[i];
					}
				}
			}
		}
		if (numarg == MESSAGE_FIELDS_COUNT && args != NULL)
		{
			id = strtoul(args[0].c_str(), NULL, 10);
			username = args[1];
			date_time = args[2];
			len = strtoul(args[3].c_str(), NULL, 10);
			state = atoi(args[4].c_str());
			body = args[5];
		}
		else
			res = false;
		return res;
	};
};