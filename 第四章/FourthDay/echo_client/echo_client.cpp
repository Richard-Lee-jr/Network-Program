// echo_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024

int _tmain(int argc, _TCHAR* argv[])
{

	if (argc != 3)
	{
		printf("arg error!");
		return -1;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup error!");
		return -1;
	}

	SOCKET srvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srvSock)
	{
		printf("socket error!");
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
		printf("connect error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	char Msg[BUF_SIZE];
	int strLen = 0;
	int sendLen = 0;
	while (true)
	{
		fputs("Input Msg(Q to quit): ", stdout);
		fgets(Msg, BUF_SIZE, stdin);

		if (!strcmp(Msg, "q\n") || !strcmp(Msg, "Q\n"))
		{
			break;
		}

		sendLen = 0;
		sendLen += send(srvSock, Msg, strlen(Msg), 0);
		//sendLen -= 1;
		sendLen += send(srvSock, Msg, strlen(Msg), 0);
		//sendLen -= 1;
		sendLen += send(srvSock, Msg, strlen(Msg), 0);
		//sendLen -= 1;
		sendLen += send(srvSock, Msg, strlen(Msg), 0);
		//sendLen -= 1;
		sendLen += send(srvSock, Msg, strlen(Msg), 0);
		//sendLen -= 1;

		//回声客户端的问题：这里send后马上recv，当send的字符串过大，服务器防分多次会发时，这里马上recv有可能没有收到全部的数据
		//延时recv解决：问题是不知延时多久。
		//Sleep(3000);
		//strLen = recv(srvSock, Msg, BUF_SIZE - 1, 0);
		//Msg[strLen] = 0;

		//正确解决：
		strLen = 0;
		while (strLen < sendLen)
		{
			int recvLen = recv(srvSock, &Msg[strLen], BUF_SIZE - 1, 0);
			if (recvLen == -1)
			{
				closesocket(srvSock);
				WSACleanup();
				return -1;
			}
			strLen += recvLen;
		}

		Msg[strLen] = 0;

		printf("Msg From Server: %s \n", Msg);
	}

	closesocket(srvSock);
	WSACleanup();
	return 0;
}

