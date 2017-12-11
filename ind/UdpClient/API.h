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
