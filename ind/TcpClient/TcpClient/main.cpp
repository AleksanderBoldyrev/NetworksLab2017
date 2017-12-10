#include <stdlib.h>
#include "ClientWorker.h"

int main(int argc, void** argv)
{
	setlocale(LC_ALL, "en_US.UTF-8");
	ClientWorker f;
	printf("Enter <host>%c<port> of Mail Server: ", DELIM);
	char s[1024];
	scanf("%s", s);
	string* ff = new string(s);

	f.StartThread(ff);
	printf("Mail client terminated!\n");
	system("pause");
	delete ff;
	return 0;
}