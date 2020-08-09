#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define OPSZ 4

void ErrorHandling(char* message);
int calculate(int opnum, int opnds[], char oprator);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	char opinfo[BUF_SIZE];
	int result, opndCnt, i;
	int recvCnt, recvLen;
	SOCKADDR_IN servAdr, clntAdr;

	int clntAdrSize;
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// 소켓 생성
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	// IP주소와 PORT 번호의 할당
	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");
	clntAdrSize = sizeof(clntAdr);

	// 총 5개의 클라이언트 연결요청을 수용하기 위해
	for (i = 0; i < 5; i++)
	{
		opndCnt = 0;

		// 클라이언트 프로그램에서의 연결요청을 수락할 때 호출하는 함수
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);
		// 클라이언트로부터 피연산자의 개수정보 수신
		recv(hClntSock, &opndCnt, 1, 0);

		recvLen = 0;
		// 피연산자의 개수정보를 바탕으로 피연산자 정보를 수신
		while ((opndCnt * OPSZ + 1) > recvLen)
		{
			recvCnt = recv(hClntSock, &opinfo[recvLen], BUF_SIZE - 1, 0);
			recvLen += recvCnt;
		}
		result = calculate(opndCnt, (int*)opinfo, opinfo[recvLen - 1]);
		// 계산한 결과값을 클라이언트에 전송
		send(hClntSock, (char*)&result, sizeof(result), 0);
		closesocket(hClntSock);
	}
	
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

// 계산
int calculate(int opnum, int opnds[], char op)
{
	int result = opnds[0], i;

	switch (op)
	{
	case '+':
		for (i = l; i < opnum; i++)
			result += opnds[i];
		break;
	case '-':
		for (i = l; i < opnum; i++)
			result -= opnds[i];
		break;
	case '*':
		for (i = l; i < opnum; i++)
			result *= opnds[i];
		break;
	}
	return result;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}