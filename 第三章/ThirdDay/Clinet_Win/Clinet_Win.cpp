// Clinet_Win.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define BUF_SIZE 1024

int _tmain(int argc, char* argv[])
{
	if (argc != 3)
	{
		return -1;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}

	SOCKET srvSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == srvSocket)
	{
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	if ( SOCKET_ERROR == connect(srvSocket, (sockaddr*)&srvAddr, sizeof(srvAddr)) )
	{
		closesocket(srvSocket);
		WSACleanup();
		return -1;
	}

	puts("Connected...");

	char msg[BUF_SIZE];
	int strLen = 0;
	while (1)
	{
		fputs("Input Message(Q to quit): ", stdout);
		fgets(msg, BUF_SIZE, stdin);

		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			printf("quit connect \n");
			break;
		}
		send(srvSocket, msg, strlen(msg), 0);
		strLen = recv(srvSocket, msg, BUF_SIZE - 1, 0);
		msg[BUF_SIZE] = '\0';
		printf("Message From Server: %s \n", msg);
	}

	closesocket(srvSocket);
	WSACleanup();
	printf("application end \n");
	return 0;
}

