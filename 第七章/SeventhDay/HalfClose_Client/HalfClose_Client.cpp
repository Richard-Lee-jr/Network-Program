// HalfClose_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 30


int _tmain(int argc, _TCHAR* argv[])
{
	if(argc != 3)
	{
		printf("argc error!\n");
		return -1;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup error!\n");
		return -1;
	}

	SOCKET srvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srvSock)
	{
		printf("socket error!\n");
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	if (SOCKET_ERROR == connect(srvSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		printf("connect error!\n");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	int nRecvLen = 0;
	char Msg[BUF_SIZE];
	while ( (nRecvLen = recv(srvSock, Msg, BUF_SIZE, 0)) != 0 )
	{
		Msg[nRecvLen] = 0;
		printf("Recv From Server: %s\n", Msg);
	}

	//复位器半关闭后
	send(srvSock, "thank you", 10, 0);
	closesocket(srvSock);
	WSACleanup();

	fputs("任意键结束...", stdout);
	getchar();

	return 0;
}