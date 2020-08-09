#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(char* message);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	char opmsg[BUF_SIZE];
	int result, opndCnt, i;
	SOCKADDR_IN servAdr;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <post>\n", argv[0]);
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
	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	servAdr.sin_port = htons(atoi(argv[2]));

	// 서버에 연결 요청(bind 없이 소켓에 POST와 IP 할당)
	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");
	else
		puts("Connected.........");

	// 피연산자 개수 입력
	fputs("Operand count : ", stdout);
	scanf("%d", &opndCnt);
	opmsg[0] = (char)opndCnt;

	// 피연산자 입력
	for (i = 0; i < opndCnt; i++)
	{
		printf("Operand %d : ", i + 1);
		scanf("%d", (int*)&opmsg[i*OPSZ+1]);
	}

	// 버퍼에 남아있는 \n 문자 삭제
	fgetc(stdin);		
	fputs("Operator : ", stdout);
	// 연산자 정보 입력
	scanf("%c", &opmsg[opndCnt * OPSZ + 1]);
	
	// 전송
	send(hSocket, opmsg, opndCnt * OPSZ + 2, 0);
	// 결과 받기
	recv(hSocket, &result, RLT_SIZE, 0);

	printf("Operation result : %d \n", result);
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