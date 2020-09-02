#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 30
void ErrorHandling(char* message);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAdr;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <post>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// 소켓 생성
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family = AF_INET;
	sendAdr.sin_addr.s_addr = inet_addr(argv[1]);
	sendAdr.sin_port = htons(atoi(argv[2]));

	// 서버에 연결 요청(bind 없이 소켓에 POST와 IP 할당)
	if (connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");

	send(hSocket, "123", 3, 0);
	send(hSocket, "4", 1, MSG_OOB);
	send(hSocket, "567", 3, 0);
	send(hSocket, "890", 3, MSG_OOB);
	
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

/*
긴급 메시지를 전송하는 코드
*/