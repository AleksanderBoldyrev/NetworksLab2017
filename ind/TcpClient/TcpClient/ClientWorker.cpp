#include "ClientWorker.h"

ClientWorker::ClientWorker()
{}

ClientWorker::~ClientWorker()
{}

DWORD WINAPI ClientWorker::HandleThread(LPVOID args)
{
	string* c = (string*)args;
	if (c != nullptr)
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
					instance.Run(host, port);
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
	DWORD t;
	tHandle = CreateThread(0, 0, HandleThread, params, 0, &t);
	WaitForSingleObject(tHandle, INFINITE);
}

void ClientWorker::StopThread()
{
	CloseHandle(tHandle);
}

string Message::Serialize()
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
}

void Message::Clear()
{
	id = 0;
	username = "";
	date_time = "";
	len = 0;
	state = MSTATE_NORMAL;
	body = "";
};

bool Message::Deserialize(const string& input)
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
		id = stoul(args[0].c_str(), NULL, 10);
		username = args[1];
		date_time = args[2];
		len = stoul(args[3].c_str(), NULL, 10);
		state = atoi(args[4].c_str());
		body = args[5];
	}
	else
		res = false;
	return res;
};

void ClientWorker::ListenLoop(const int& socket)
{
	string buffer;
	short state = 0;
	int error = 0;
	int c = 0;
	state = STATE::START;
	string buf;
	unsigned int answerCode;
	unsigned short numArgCount;
	string* args = NULL;

	bool isLogged = false;
	string log;
	string pass;
	bool dr = false;
	bool df = false;
	bool ds = false;

	string uname;
	Message m;
	string mes;
	int mesId = -1;

	while (1)
	{
		error = WSAGetLastError();
		if (error != 0)
		{
			printf("Socket error: %d", error);
			break;
		}
		switch (state)
		{
		case STATE::START:
			SendTo(socket, Serialize(STATE::START, 0, nullptr));
			ListenRecv(socket, buf);
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != STATE::NO_OPERATION)
			{
				if (numArgCount > 0 && args != nullptr)
				{
					if (atoi(args[0].c_str()) == STATE::SERV_OK)
					{
						cout << "Connected to server successfully." << endl;
						state = STATE::INIT;
					}
					else
					{
						cout << "Smth went wrong [stcmp]" << endl;
						_getch();
					}
				}
				else
				{
					cout << "Smth went wrong [numarg || args]" << endl;
					_getch();
				}
			}
			else
			{
				cout << "Smth went wrong [ansCode]" << endl;
				_getch();
			}
			break;
		case STATE::NO_OPERATION:
			SendTo(socket, Serialize(STATE::NO_OPERATION, 0, nullptr));
			ListenRecv(socket, buf);
			answerCode = Parse(buf, numArgCount, args);
			cout << "Enter a valid operation number.\n";
			break;
		case STATE::ANSWER:
			cout << "Got the ANSWER message from server." << endl;
			break;
		case STATE::INIT:
			df = false;
			while (!df)
			{
				cout << "* MAIL *\n" << "Select the following items:\n" << "2 - Exit\n" << "3 - Register\n" << "4 - Login\n" << OPENT << endl;
				short op; //= _getch() - (int)'0';
				cin >> op;
				df = true;
				switch (op)
				{
				case 2:
					state = STATE::EXIT;
					break;
				case 3:
					state = STATE::REG;
					break;
				case 4:
					state = STATE::LOG;
					break;
				default:
					df = false;
					printf("Not valid operation number.");
					_getch();
				}
			}
			break;
		case STATE::OPCODE:

			break;
		case STATE::EXIT:
			SendTo(socket, Serialize(STATE::EXIT, 0, nullptr));
			ListenRecv(socket, buf);
			answerCode = Parse(buf, numArgCount, args);
			return;
			break;
		case STATE::REG:
			dr = false;
			while (!dr)
			{
				cout << "You are about to sign up. Enter the <username>: ";
				cin >> log;
				cout << "Enter the <password>: ";
				cin >> pass;
				if (log.size() > 0 && pass.size() > 0)
				{
					string* bufs = new string[2];
					bufs[0] = log;
					bufs[1] = pass;
					SendTo(socket, Serialize(STATE::REG, 2, bufs));
					delete[] bufs;
					ListenRecv(socket, buf);
					answerCode = Parse(buf, numArgCount, args);
					if (answerCode != STATE::NO_OPERATION)
					{
						if (numArgCount > 0 && args != nullptr)
						{
							if (atoi(args[0].c_str()) == STATE::SERV_OK)
							{
								dr = true;
								state = STATE::INIT;
								cout << "User created successfully. Press any key.\n" << endl;
								_getch();
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
						if (numArgCount > 0 && args != nullptr)
							cout << "ERROR: while creating user err[" << args[0] << "]\n";
						else cout << "Unknown error.\n";
						_getch();
					}
				}
				else
				{
					printf("Login or password missing.\n");
					_getch();
				}
			}
			break;
		case STATE::LOG:
			dr = false;
			while (!dr)
			{
				cout << "You are about to sign in. Enter the <username>: ";
				cin >> log;
				cout << "Enter the <password>: ";
				cin >> pass;
				if (log.size() > 0 && pass.size() > 0)
				{
					string* bufs = new string[2];
					bufs[0] = log;
					bufs[1] = pass;
					SendTo(socket, Serialize(STATE::LOG, 2, bufs));
					delete[] bufs;
					ListenRecv(socket, buf);
					answerCode = Parse(buf, numArgCount, args);
					if (answerCode != STATE::NO_OPERATION)
					{
						if (numArgCount > 0 && args != nullptr)
						{
							if (atoi(args[0].c_str()) == STATE::SERV_OK)
							{
								dr = true;
								state = STATE::INSYS;
								cout << "User signed in successfully. Press any key.\n" << endl;
								_getch();
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
						if (numArgCount > 0 && args != nullptr)
							cout << "ERROR: while logging in err[" << args[0] << "]\n";
						else cout << "Unknown error.\n";
						_getch();
					}
				}
				else
				{
					printf("Login or password missing.\n");
					_getch();
				}
			}
			break;
		case STATE::INSYS:
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
					state = STATE::SND;
					break;
				case 2:
					state = STATE::EXIT;
					break;
				case 3:
					state = STATE::REG;
					break;
				case 4:
					state = STATE::LOGOUT;
					break;
				case 5:
					state = STATE::DEL_US;
					break;
				case 6:
					state = STATE::SH_UNR;
					break;
				case 7:
					state = STATE::SH_ALL;
					break;
				case 8:
					state = STATE::SH_EX;
					break;
				case 9:
					state = STATE::DEL_MES;
					break;
				case 10:
					state = STATE::RSND;
					break;
				default:
					ds = false;
					printf("Not valid operation number.");
					_getch();
				}
			}
			break;
		case STATE::LOGOUT:
			cout << "Logging out.\n" << endl;
			SendTo(socket, Serialize(STATE::LOGOUT, 0, nullptr));
			ListenRecv(socket, buf);
			answerCode = Parse(buf, numArgCount, args);
			if (atoi(args[0].c_str()) == STATE::SERV_OK)
			{
				cout << "Log out successfully. Press any key." << endl;
				state = STATE::INIT;
			}
			else cout << "Error while logging out. Press any key." << endl;
			_getch();
			break;
		case STATE::SND:
			cout << "Sending the message. Enter the username of the user you would like to send: " << endl;
			cin >> uname;
			cout << "Sending the message. Enter the message to send: " << endl;
			cin >> mes;
			m.Clear();
			m.body = mes;
			if (uname.size() > 0)
			{
				string* bufs = new string[2];
				bufs[0] = uname;
				bufs[1] = m.Serialize();
				SendTo(socket, Serialize(STATE::SND, 2, bufs));
				delete[] bufs;
				ListenRecv(socket, buf);
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != STATE::NO_OPERATION)
				{
					if (numArgCount > 0 && args != nullptr)
					{
						if (atoi(args[0].c_str()) == STATE::SERV_OK)
						{
							if (m.Deserialize(args[1]))
							{
								m.body = mes;
								cout << "Message successfully sent to user " << uname << endl;
								cout << MessageToString(m) << endl;
								cout << "\nPress any key." << endl;
								state = STATE::INSYS;
								_getch();
							}
						}
						else
						{
							if (numArgCount > 1)
							{
								cout << "ERROR: while sending [" << args[1] << "]\n";
							}
							else cout << "Error while sending.";
							state = STATE::INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != nullptr)
						cout << "ERROR: while sending err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					state = STATE::INSYS;
					_getch();
				}
			}
			break;
		case STATE::DEL_US:
			cout << "Deleting user." << endl;
			SendTo(socket, Serialize(STATE::DEL_US, 0, nullptr));
			ListenRecv(socket, buf);
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != STATE::NO_OPERATION)
			{
				if (numArgCount > 0 && args != nullptr)
				{
					if (atoi(args[0].c_str()) == STATE::SERV_OK)
					{
						cout << "User <" << args[1] << "> deleted successfully." << endl;
						cout << "\nPress any key." << endl;
						state = STATE::INIT;
						_getch();
					}
					else
					{
						if (numArgCount > 1)
						{
							cout << "ERROR: while deleting [" << args[1] << "]\n";
						}
						else cout << "Error while deleting.";
						state = STATE::INSYS;
					}
				}
			}
			else
			{
				if (numArgCount > 0 && args != nullptr)
					cout << "ERROR: while deleting err[" << args[0] << "]\n";
				else cout << "Unknown error.\n";
				state = STATE::INSYS;
				_getch();
			}
			break;
		case STATE::DEL_MES:
			cout << "Deleting message." << endl;
			cout << "Enter the message ID which you would like to delete:" << endl;
			cin >> mesId;
			if (mesId > 0)
			{
				stringstream sss;
				string bufs[1];
				sss << mesId;
				bufs[0] = sss.str();
				SendTo(socket, Serialize(STATE::DEL_MES, 1, bufs));
				ListenRecv(socket, buf);
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != STATE::NO_OPERATION)
				{
					if (numArgCount > 0 && args != nullptr)
					{
						if (atoi(args[0].c_str()) == STATE::SERV_OK)
						{
							cout << "Message <" << mesId << "> deleted successfully." << endl;
							cout << "\nPress any key." << endl;
							state = STATE::INSYS;
							_getch();
						}
						else
						{
							if (numArgCount > 1)
							{
								cout << "ERROR: while deleting [" << args[1] << "]\n";
							}
							else cout << "Error while deleting.";
							state = STATE::INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != nullptr)
						cout << "ERROR: while deleting err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					state = STATE::INSYS;
					_getch();
				}
			}
			break;
		case STATE::SH_UNR:
			cout << "Showing unread messages." << endl;
			SendTo(socket, Serialize(STATE::SH_UNR, 0, nullptr));
			ListenRecv(socket, buf);
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != STATE::NO_OPERATION)
			{
				if (numArgCount > 0 && args != nullptr)
				{
					if (atoi(args[0].c_str()) == STATE::SERV_OK)
					{
						for (int i = 1; i < numArgCount; i++)
						{
							if (m.Deserialize(args[i]))
							{
								cout << "Unread message with ID = " << m.id << endl;
								cout << MessageToString(m) << endl;
							}
						}
						cout << "\nPress any key." << endl;
						state = STATE::INSYS;
						_getch();

					}
					else
					{
						if (numArgCount > 1)
						{
							cout << "ERROR: while showing unread messages [" << args[1] << "]\n";
						}
						else cout << "Error while showing unread messages.";
						state = STATE::INSYS;
					}
				}
			}
			else
			{
				if (numArgCount > 0 && args != nullptr)
					cout << "ERROR: while showing unread messages err[" << args[0] << "]\n";
				else cout << "Unknown error.\n";
				state = STATE::INSYS;
				_getch();
			}
			break;
		case STATE::SH_ALL:
			cout << "Showing all messages." << endl;
			SendTo(socket, Serialize(STATE::SH_ALL, 0, nullptr));
			ListenRecv(socket, buf);
			answerCode = Parse(buf, numArgCount, args);
			if (answerCode != STATE::NO_OPERATION)
			{
				if (numArgCount > 0 && args != nullptr)
				{
					if (atoi(args[0].c_str()) == STATE::SERV_OK)
					{
						for (int i = 1; i < numArgCount; i++)
						{
							if (m.Deserialize(args[i]))
							{
								cout << "Message with ID = " << m.id << endl;
								cout << MessageToString(m) << endl;
							}
						}
						cout << "\nPress any key." << endl;
						state = STATE::INSYS;
						_getch();

					}
					else
					{
						if (numArgCount > 1)
						{
							cout << "ERROR: while showing all messages [" << args[1] << "]\n";
						}
						else cout << "Error while showing all messages.";
						state = STATE::INSYS;
					}
				}
			}
			else
			{
				if (numArgCount > 0 && args != nullptr)
					cout << "ERROR: while showing unread messages err[" << args[0] << "]\n";
				else cout << "Unknown error.\n";
				state = STATE::INSYS;
				_getch();
			}
			break;
		case STATE::SH_EX:
			cout << "Showing the exact message." << endl;
			cout << "Enter the message ID which you would like to get: " << endl;
			cin >> mesId;
			if (mesId > 0)
			{
				stringstream sss;
				string bufs[1];
				sss << mesId;
				bufs[0] = sss.str();
				SendTo(socket, Serialize(STATE::SH_EX, 1, bufs));
				ListenRecv(socket, buf);
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != STATE::NO_OPERATION)
				{
					if (numArgCount > 0 && args != nullptr)
					{
						if (atoi(args[0].c_str()) == STATE::SERV_OK)
						{
							if (m.Deserialize(args[1]))
							{
								cout << "Message with ID = " << m.id << endl;
								cout << MessageToString(m) << endl;
							}
							cout << "\nPress any key." << endl;
							state = STATE::INSYS;
							_getch();

						}
						else
						{
							if (numArgCount > 1)
							{
								cout << "ERROR: while showing unread messages [" << args[1] << "]\n";
							}
							else cout << "Error while showing unread messages.";
							state = STATE::INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != nullptr)
						cout << "ERROR: while showing unread messages err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					state = STATE::INSYS;
					_getch();
				}
			}
			break;
		case STATE::RSND:
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
				SendTo(socket, Serialize(STATE::RSND, 2, bufs));
				ListenRecv(socket, buf);
				answerCode = Parse(buf, numArgCount, args);
				if (answerCode != STATE::NO_OPERATION)
				{
					if (numArgCount > 1 && args != nullptr)
					{
						if (atoi(args[0].c_str()) == STATE::SERV_OK)
						{
							if (m.Deserialize(args[1]))
							{
								cout << "Message successfully resent to user " << uname << endl;
								cout << MessageToString(m) << endl;
								cout << "\nPress any key." << endl;
								state = STATE::INSYS;
								_getch();
							}
						}
						else
						{
							cout << "ERROR: while resending [" << args[1] << "]\n";
							state = STATE::INSYS;
						}
					}
				}
				else
				{
					if (numArgCount > 0 && args != nullptr)
						cout << "ERROR: while resending err[" << args[0] << "]\n";
					else cout << "Unknown error.\n";
					_getch();
				}
			}
			break;
		default:
			printf("Smth went wrong [non existant state].");
			_getch();
		}
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

string ClientWorker::Serialize(STATE opcode, unsigned short numarg, const string* ss)
{
	stringstream sstr;
	sstr << (int)opcode << DELIM_PARSE << (int)numarg << DELIM_PARSE;
	if (numarg > 0 && ss != nullptr)
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
	STATE res = STATE::NO_OPERATION;
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
	return STATE::NO_OPERATION;
}

void ClientWorker::Run(string host, unsigned short port)
{
	printf("Starting new client thread with HOST=%s, PORT=%u\n", host.c_str(), port);
	int n;
	WSADATA wsaData;
	n = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (n != 0) {
		printf("WSAStartup failed: %d\n", n);
	}
	else
	{
		int sockfd;
		struct sockaddr_in serv_addr;
		struct hostent *server;

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			perror("ERROR opening socket");
		}
		else
		{
			server = gethostbyname(host.c_str());
			if (server == NULL) {
				fprintf(stderr, "ERROR, no such host\n");
			}
			else
			{
				memset((char*)&serv_addr, 0, sizeof(serv_addr));
				serv_addr.sin_family = AF_INET;
				memcpy((char *)&serv_addr.sin_addr.s_addr, server->h_addr, (size_t)server->h_length);
				serv_addr.sin_port = htons(port);
				if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {

				}
				else
				{
					ListenLoop(sockfd);
					n = shutdown(sockfd, SD_BOTH);
					if (n == SOCKET_ERROR) {
						printf("shutdown failed: %d\n", WSAGetLastError());
						closesocket(sockfd);
						WSACleanup();
					}
					else printf("Connection closed successfully. Bye!\n");
				}
				closesocket(sockfd);
				WSACleanup();
			}
		}
	}
}

void ClientWorker::SendTo(SOCKET socket, const string& message)
{
	cout << "Send to server: " << message << endl;
	int res = 0;
	int size = message.size();
	stringstream ss;
	ss << size;
	string s = ss.str();
	while (s.size() < 10)
	{
		s.insert(s.begin(), '0');
	}
	s += message;
	res = send(socket, s.c_str(), s.size(), 0);
	if (res != s.size())
		printf("Send failed: %d != %d!\n", res, s.size());
}

bool ClientWorker::ListenRecv(SOCKET socket, std::string& MsgStr)
{
	char* c = new char[BUFSIZE];
	unsigned int size = 0;
	int ssize = recv(socket, c, BUFSIZE, 0);
	if (ssize == 10)
	{
		size = atoi(c);
		char* recvbuf = new char[size];

		int res = recv(socket, recvbuf, size, 0);
		if (res > 0)
		{
			MsgStr.clear();
			for (int i = 0; i < res; i++)
				if (recvbuf[i] != '\n' && recvbuf[i] != '\r' && recvbuf[i] != '\t' && recvbuf[i] != '\0')
					MsgStr.push_back(recvbuf[i]);
		}
	}
	else
		return false;
	cout << "Recieved: " << MsgStr << endl;
	return true;
}
