#include <stdio.h>
#include <WinSock2.h>

int main(int argc, char *argv[])
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		_ERRHANDLING_H_("WSAStartup() error!");

	return 0;
}