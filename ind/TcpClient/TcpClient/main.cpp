#include <stdlib.h>
#include "ClientWorker.h"

int main(int argc, void** argv)
{
	setlocale(LC_ALL, "en_US.UTF-8");
	ClientWorker f;
	while (true)
	{
		printf("Enter <host>%c<port> of Mail Server: ", DELIM);
		char s[1024];
		scanf("%s", s);
		string* ff = new string(s);
		f.startThread(ff);
		Sleep(5000); // wait 5 seconds until client is connecting to server
		while (true)
		{
			if (f.isRunning == false)
				break;
			Sleep(1000);
		}
		printf("Mail client terminated!\n");
	}
	system("pause");
	return 0;
}