#include "ClientWorker.h"

ClientWorker::ClientWorker()
{

}

ClientWorker::~ClientWorker()
{}

void* ClientWorker::HandleThread(void* args)
{
	string* c = (string*)args;
	if (c != NULL)
	{
		string s = *c;
		string host = "";
		unsigned short port = 0;
		bool divOccured = true;
		string buf = "";
		if (s.size() > 0)
		{
			for (unsigned long i = 0; i < s.size(); i++)
			{
				if (s[i] == DELIM)
					divOccured = false;
				else
				{
					if (divOccured)
						host += s[i];
					else
						buf += s[i];
				}
			}
			if (host.size() > 0 && buf.size() > 0)
			{
				port = atoi(buf.c_str());
				if (port > 0)
				{
					static ClientWorker instance;
                                        instance.ListenLoop(host, port);
				}
				else
					printf("Port is not valid.");
			}
			else
				printf("Missing host or port definition.");
		}
		else
			printf("Missing the data.");
	}
	return 0;
}

void ClientWorker::StartThread(string* params)
{
        pthread_create(&tHandle, 0, HandleThread, (void *) params);
        pthread_join(tHandle, NULL);
}

void ClientWorker::stopThread()
{
	pthread_cancel(tHandle);
}

void ClientWorker::ListenLoop(string host, unsigned short port)
{
    // init network
    
    server = gethostbyname(host.c_str());
    
    bzero((char *) &servOut, sizeof(servOut));
    servOut.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &servOut.sin_addr.s_addr, (size_t) server->h_length);
    servOut.sin_port = htons(port);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        
	short state = 0;
	int error = 0;
	state = START;
	string buf;
	
	string log;
	string pass;

	bool df = false;
	bool ds = false;

	Message m;
	string mes;


	while (1)
	{
		if (error != 0)
		{
			printf("Socket error: %d", error);
			break;
		}
		switch (state)
		{
		case START:
			SendTo(sockfd, Serialize(START, 0, NULL));
			ListenRecv(sockfd, buf);
			break;
		case NO_OPERATION:
			SendTo(sockfd, Serialize(NO_OPERATION, 0, NULL));
			ListenRecv(sockfd, buf);
			break;
		case ANSWER:
			cout << "Got the ANSWER message from server." << endl;
			break;
		case INIT:
			df = false;
			while (!df)
			{
				cout << "* MAIL *\n" << "Select the following items:\n" << "2 - Exit\n" << "3 - Register\n" << "4 - Login\n" << OPENT << endl;
				short op;
				cin >> op;
				df = true;
				switch (op)
				{
				case 2:
					state = EXIT;
					break;
				case 3:
					state = REG;
					break;
				case 4:
					state = LOG;
					break;
				default:
					df = false;
					printf("Not valid operation number.");
					getchar();
				}
			}
                        continue;
			break;
		case EXIT:
			SendTo(sockfd, Serialize(EXIT, 0, NULL));
			ListenRecv(sockfd, buf);
			break;
		case REG:
				cout << "You are about to sign up. Enter the <username>: ";
				cin >> log;
				cout << "Enter the <password>: ";
				cin >> pass;
				if (log.size() > 0 && pass.size() > 0)
				{
					string * bufs = new string[2];
					bufs[0] = log;
					bufs[1] = pass;
					SendTo(sockfd, Serialize(REG, 2, bufs));
					delete[] bufs;
					ListenRecv(sockfd, buf);
				}
				else
				{
					printf("Login or password missing.\n");
					getchar();
				}
			break;
		case LOG:
			dr = false;
				cout << "You are about to sign in. Enter the <username>: ";
				cin >> log;
				cout << "Enter the <password>: ";
				cin >> pass;
				if (log.size() > 0 && pass.size() > 0)
				{
					string * bufs = new string[2];
					bufs[0] = log;
					bufs[1] = pass;
					SendTo(sockfd, Serialize(LOG, 2, bufs));
					delete[] bufs;
					ListenRecv(sockfd, buf);
				}
				else
				{
					printf("Login or password missing.\n");
					getchar();
				}
			break;
		case INSYS:
			ds = false;
			while (!ds)
			{
				cout << "* MAIL *\n" << "Select the following items:\n" << "1 - Send message\n" << "2 - Exit\n" << "3 - Register\n" << "4 - Logout\n" << "5 - Delete user\n" << "6 - Show unread messages\n" << "7 - Show all messages\n" << "8 - Show the exact message\n" << "9 - Delete message\n" << "10 - Resend message\n" << endl;
				short op;
				cin >> op;
				ds = true;
				switch (op)
				{
				case 1:
					state = SND;
					break;
				case 2:
					state = EXIT;
					break;
				case 3:
					state = REG;
					break;
				case 4:
					state = LOGOUT;
					break;
				case 5:
					state = DEL_US;
					break;
				case 6:
					state = SH_UNR;
					break;
				case 7:
					state = SH_ALL;
					break;
				case 8:
					state = SH_EX;
					break;
				case 9:
					state = DEL_MSG;
					break;
				case 10:
					state = RSND;
					break;
				default:
					ds = false;
					printf("Not valid operation number.");
					getchar();
				}
			}
                        continue;
			break;
		case LOGOUT:
			cout << "Logging out.\n" << endl;
			SendTo(sockfd, Serialize(LOGOUT, 0, NULL));
			ListenRecv(sockfd, buf);
			break;
		case SND:
			cout << "Sending the message. Enter the username of the user you would like to send: " << endl;
			cin >> uname;
			cout << "Sending the message. Enter the message to send: " << endl;
			cin >> mes;
			m.clear();
			m.body = mes;
			if (uname.size() > 0)
			{
				string * bufs = new string[2];
				bufs[0] = uname;
				bufs[1] = m.serialize();
				SendTo(sockfd, Serialize(SND, 2, bufs));
				delete[] bufs;
				ListenRecv(sockfd, buf);
			}
			break;
		case DEL_US:
			cout << "Deleting user." << endl;
			SendTo(sockfd, Serialize(DEL_US, 0, NULL));
			ListenRecv(sockfd, buf);
			break;
		case DEL_MSG:
			cout << "Deleting message." << endl;
			cout << "Enter the message ID which you would like to delete:" << endl;
			cin >> mesId;
			if (mesId > 0)
			{
				stringstream sss;
				string bufs[1];
				sss << mesId;
				bufs[0] = sss.str();
				SendTo(sockfd, Serialize(DEL_MSG, 1, bufs));
				ListenRecv(sockfd, buf);
			}
			break;
		case SH_UNR:
			cout << "Showing unread messages." << endl;
			SendTo(sockfd, Serialize(SH_UNR, 0, NULL));
			ListenRecv(sockfd, buf);
			break;
		case SH_ALL:
			cout << "Showing all messages." << endl;
			SendTo(sockfd, Serialize(SH_ALL, 0, NULL));
			ListenRecv(sockfd, buf);
			break;
		case SH_EX:
			cout << "Showing the exact message." << endl;
			cout << "Enter the message ID which you would like to get: " << endl;
			cin >> mesId;
			if (mesId > 0)
			{
				stringstream sss;
				string bufs[1];
				sss << mesId;
				bufs[0] = sss.str();
				SendTo(sockfd, Serialize(SH_EX, 1, bufs));
				ListenRecv(sockfd, buf);
			}
			break;
		case RSND:
			cout << "Resending the exact message." << endl;
			cout << "Resending the message. Enter the username of the user you would like to send: " << endl;
			cin >> uname;
			cout << "Enter the message ID to resend: " << endl;
			cin >> mesId;
			if (uname.size() > 0)
			{
				stringstream sss;
				string bufs[2];
				sss << mesId;
				bufs[0] = sss.str();
				bufs[1] = uname;
				SendTo(sockfd, Serialize(RSND, 2, bufs));
				ListenRecv(sockfd, buf);
			}
			break;
		default:
			printf("Smth went wrong [non existant state].");
			getchar();
		}
                processRes(state, buf, m, mes);
	}
}

void ClientWorker::processRes(short& state, const string& buf, Message& m, const string& mes)
{
    unsigned int answerCode;
    unsigned short numArgCount;
    string* args = NULL;
        
    switch (state)
		{
		case START:
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != NO_OPERATION)
			{
				if (numArgCount > 0 && args != NULL)
				{
					if (atoi(args[0].c_str()) == SERV_OK)
					{
						cout << "Connected to server successfully." << endl;
						state = INIT;
						getchar();
					}
					else
					{
						cout << "Smth went wrong [stcmp]" << endl;
						getchar();
					}
				}
				else
				{
					cout << "Smth went wrong [numarg || args]" << endl;
					getchar();
				}
			}
			else
			{
				cout << "Smth went wrong [ansCode]" << endl;
				getchar();
			}
			break;
		case NO_OPERATION:
			answerCode = Parse(buf, numArgCount, args);
			cout << "Enter a valid operation number.\n";
			break;
		case ANSWER:
			cout << "Got the ANSWER message from server." << endl;
			break;
		case EXIT:
			answerCode = Parse(buf, numArgCount, args);
			return;
			break;
		case REG:
					answerCode = Parse(buf, numArgCount, args);
					if (answerCode != NO_OPERATION)
					{
						if (numArgCount > 0 && args != NULL)
						{
							if (atoi(args[0].c_str()) == SERV_OK)
							{
								state = INIT;
								cout << "User created successfully. Press any key.\n" << endl;
								getchar();
							}
							else
							{
								if (numArgCount > 1)
								{
									cout << "ERROR: while signing up" << args[1] << "]\n";
								}
								else cout << "Error while signing up.";
							}
						}
					}
					else
					{
						if (numArgCount > 0 && args != NULL)
							cout << "ERROR: while creating user err[" << args[0] << "]\n";
						else cout << "Unknown error.\n";
						getchar();
					}
			break;
		case LOG:
					answerCode = Parse(buf, numArgCount, args);
					if (answerCode != NO_OPERATION)
					{
						if (numArgCount > 0 && args != NULL)
						{
							if (atoi(args[0].c_str()) == SERV_OK)
							{
								state = INSYS;
								cout << "User signed in successfully. Press any key.\n" << endl;
								getchar();
							}
							else
							{
								if (numArgCount > 1)
								{
									cout << "ERROR: while signing in" << args[1] << "]\n";
								}
								else cout << "Error while signing in.";
							}
						}
					}
					else
					{
						if (numArgCount > 0 && args != NULL)
							cout << "ERROR: while logging in err[" << args[0] << "]\n";
						else cout << "Unknown error.\n";
						getchar();
					}
			break;
		case LOGOUT:
			answerCode = Parse(buf, numArgCount, args);
			if (atoi(args[0].c_str()) == SERV_OK)
			{
				cout << "Log out successfully. Press any key." << endl;
				state = INIT;
			}
			else cout << "Error while logging out. Press any key." << endl;
			getchar();
			break;
		case SND:
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != NO_OPERATION)
				{
					if (numArgCount > 0 && args != NULL)
					{
						if (atoi(args[0].c_str()) == SERV_OK)
						{
							if (m.deserialize(args[1]))
							{
								m.body = mes;
								cout << "Message successfully sent to user " << uname << endl;
								cout << MessageToString(m) << endl;
								cout << "\nPress any key." << endl;
								state = INSYS;
								getchar();
							}
						}
						else
						{
							if (numArgCount > 1)
							{
								cout << "ERROR: while sending [" << args[1] << "]\n";
							}
							else cout << "Error while sending.";
							state = INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != NULL)
						cout << "ERROR: while sending err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					state = INSYS;
					getchar();
				}
			break;
		case DEL_US:
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != NO_OPERATION)
			{
				if (numArgCount > 0 && args != NULL)
				{
					if (atoi(args[0].c_str()) == SERV_OK)
					{
						cout << "User <" << uname << "> deleted successfully." << endl;
						cout << "\nPress any key." << endl;
						state = INIT;
						getchar();
					}
					else
					{
						if (numArgCount > 1)
						{
							cout << "ERROR: while deleting [" << args[1] << "]\n";
						}
						else cout << "Error while deleting.";
						state = INSYS;
					}
				}
			}
			else
			{
				if (numArgCount > 0 && args != NULL)
					cout << "ERROR: while deleting err[" << args[0] << "]\n";
				else cout << "Unknown error.\n";
				state = INSYS;
				getchar();
			}
			break;
		case DEL_MSG:
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != NO_OPERATION)
				{
					if (numArgCount > 0 && args != NULL)
					{
						if (atoi(args[0].c_str()) == SERV_OK)
						{
							cout << "Message <" << mesId << "> deleted successfully." << endl;
							cout << "\nPress any key." << endl;
							state = INSYS;
							getchar();
						}
						else
						{
							if (numArgCount > 1)
							{
								cout << "ERROR: while deleting [" << args[1] << "]\n";
							}
							else cout << "Error while deleting.";
							state = INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != NULL)
						cout << "ERROR: while deleting err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					state = INSYS;
					getchar();
				}
			break;
		case SH_UNR:
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != NO_OPERATION)
			{
				if (numArgCount > 0 && args != NULL)
				{
					if (atoi(args[0].c_str()) == SERV_OK)
					{
						for (int i = 1; i < numArgCount; i++)
						{
							if (m.deserialize(args[i]))
							{
								cout << "Unread message with ID = " << m.id << endl;
								cout << MessageToString(m) << endl;
							}
						}
						cout << "\nPress any key." << endl;
						state = INSYS;
						getchar();

					}
					else
					{
						if (numArgCount > 1)
						{
							cout << "ERROR: while showing unread messages [" << args[1] << "]\n";
						}
						else cout << "Error while showing unread messages.";
						state = INSYS;
					}
				}
			}
			else
			{
				if (numArgCount > 0 && args != NULL)
					cout << "ERROR: while showing unread messages err[" << args[0] << "]\n";
				else cout << "Unknown error.\n";
				state = INSYS;
				getchar();
			}
			break;
		case SH_ALL:
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != NO_OPERATION)
			{
				if (numArgCount > 0 && args != NULL)
				{
					if (atoi(args[0].c_str()) == SERV_OK)
					{
						for (int i = 1; i < numArgCount; i++)
						{
							if (m.deserialize(args[i]))
							{
								cout << "Message with ID = " << m.id << endl;
								cout << MessageToString(m) << endl;
							}
						}
						cout << "\nPress any key." << endl;
						state = INSYS;
						getchar();

					}
					else
					{
						if (numArgCount > 1)
						{
							cout << "ERROR: while showing all messages [" << args[1] << "]\n";
						}
						else cout << "Error while showing all messages.";
						state = INSYS;
					}
				}
			}
			else
			{
				if (numArgCount > 0 && args != NULL)
					cout << "ERROR: while showing unread messages err[" << args[0] << "]\n";
				else cout << "Unknown error.\n";
				state = INSYS;
				getchar();
			}
			break;
		case SH_EX:
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != NO_OPERATION)
				{
					if (numArgCount > 0 && args != NULL)
					{
						if (atoi(args[0].c_str()) == SERV_OK)
						{
							if (m.deserialize(args[1]))
							{
								cout << "Message with ID = " << m.id << endl;
								cout << MessageToString(m) << endl;
							}
							cout << "\nPress any key." << endl;
							state = INSYS;
							getchar();

						}
						else
						{
							if (numArgCount > 1)
							{
								cout << "ERROR: while showing unread messages [" << args[1] << "]\n";
							}
							else cout << "Error while showing unread messages.";
							state = INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != NULL)
						cout << "ERROR: while showing unread messages err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					state = INSYS;
					getchar();
				}
			break;
		case RSND:
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != NO_OPERATION)
				{
					if (numArgCount > 1 && args != NULL)
					{
						if (atoi(args[0].c_str()) == SERV_OK)
						{
							if (m.deserialize(args[1]))
							{
								cout << "Message successfully resent to user " << uname << endl;
								cout << MessageToString(m) << endl;
								cout << "\nPress any key." << endl;
								state = INSYS;
								getchar();
							}
						}
						else
						{
							cout << "ERROR: while resending [" << args[1] << "]\n";
							state = INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != NULL)
						cout << "ERROR: while resending err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					getchar();
				}
			break;
		default:
			printf("Smth went wrong [non existant state].");
			getchar();
		}
}

string ClientWorker::MessageToString(const Message& m)
{
	stringstream res;
	res << "ID: " << m.id << "\n";

	res << "TIME: " << m.date_time << "\n";

	res << "FROM: " << m.username << "\n";

	res << "LEN: " << m.len << "\n";

	res << "STATE: " << MESSAGE_STATES[m.state] << "\n";

	res << "BODY: " << m.body << "\n";
	return res.str();
}

string ClientWorker::Serialize(unsigned int opcode, unsigned short numarg, const string * ss)
{
	stringstream sstr;
	sstr << (int)opcode << DELIM_PARSE << (int)numarg << DELIM_PARSE;
	if (numarg > 0 && ss != NULL)
		for (int i = 0; i < numarg; i++)
		{
			string temp = ss[i];
			std::replace(temp.begin(), temp.end(), DELIM_PARSE, ' ');
			sstr << temp << DELIM_PARSE;
		}
	return sstr.str();
}

STATE ClientWorker::Parse(const string& input, unsigned short& numarg, string* &args)
{
	STATE res = NO_OPERATION;
	if (input.size() > 0)
	{
		stringstream buf;
		numarg = 0;
		// find all delimeters
		for (int i = 0; i < input.size(); i++)
		{
			if (input[i] == DELIM_PARSE)
				numarg++;
		}
		// find all parts
		if (numarg > 0)
		{
			args = new string[numarg - 1];
			string opcodeBuf;
			unsigned short cc = 0;
			for (int i = 0; i < input.size(); i++)
			{
				if (input[i] == DELIM_PARSE)
				{
					if (cc == 0)
					{
						opcodeBuf = buf.str();
					}
					else if (cc > 1)
					{
						args[cc - 2] = buf.str();
					}
					cc++;
					buf.str(std::string());
				}
				else
				{
					buf << input[i];
				}
			}
			// args[0] is operation code
			res = ParseOpCode(opcodeBuf);
			numarg -= 2;
		}
	}
	return res;
}

STATE ClientWorker::ParseOpCode(const string& buf)
{
        int res = atoi(buf.c_str());
	if (API_SIZE > 0)
		for (int i = 0; i < API_SIZE; i++)
			if (res == i)
				return static_cast<STATE>(i);
	return NO_OPERATION;
}

void ClientWorker::SendTo(int socket, const string& temp) 
{
    if (temp.size() > 0) 
    {
        string message = temp + EOF_SYM;
        int tolen = sizeof (servOut);
        string sndBuf;
        long int curPos = 0;
        while (curPos < message.length()) 
        {
            int num = ++lastPacketNumSend;
            sndBuf = intToStr(num);
            int tLen = min((int)(message.length()-curPos), UDP_DG_LEN - TECH_DG_SIZE);
            sndBuf += message.substr(curPos, tLen);
            curPos += tLen;
            cout << "Sending: " << sndBuf << endl;
            int count = sendto(socket, sndBuf.c_str(), sndBuf.length(), 0, (sockaddr*) & servOut, tolen);
            if (count != sndBuf.length())
                cout << "Send data mismatch: send " << count << ", have " << sndBuf.length() << endl;
        }
    }
}

bool ClientWorker::ListenRecv(int socket, std::string& MsgStr) {
    char buffer[UDP_DG_LEN];
    size_t len = TECH_DG_SIZE;
    size_t res = 0;
    string s;
    MsgStr.clear();
    memset(buffer, 0, sizeof(buffer));
    while (res = recvfrom(socket, buffer, UDP_DG_LEN, 0, (struct sockaddr *) &servIn, &servIn_size) > 0) {
        s.clear();
        for (int i=0; i<UDP_DG_LEN; i++)
        {
            s+=buffer[i];
            if (buffer[i]==EOF_SYM)
                break;
        }
        memset(buffer, 0, sizeof(buffer));
        s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
        s.erase(std::remove(s.begin(), s.end(), '\0'), s.end());
        cout << "Recieved: " << s << endl;
        string str = s;
        size_t num = 0;
        if (s.length() > TECH_DG_SIZE) {
            string c = str.substr(0, TECH_DG_SIZE);
            str = str.substr(TECH_DG_SIZE, str.length() - TECH_DG_SIZE);
            num = atoi(c.c_str());
        }
        if (lastPacketNumRecv + 1 == num) {
             lastPacketNumRecv = num;
            for (int i = 0; i < str.length(); i++) {
                if (str[i] == EOF_SYM) 
                {                    
                    MsgStr.append(tempRBuf);
                    servOut = servIn;
                    tempRBuf = "";
                    return true;
                } else
                    tempRBuf += str[i];
            }
        }
        else
            cout << "Packet order mismatch!\n";
    }
    return true;
}
