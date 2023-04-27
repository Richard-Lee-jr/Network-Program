// Server_Win.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024

int _tmain(int argc, char* argv[])
{
	if (argc != 2)
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
	srvAddr.sin_addr.s_addr = htonl(ADDR_ANY);
	srvAddr.sin_port = htons(_ttoi(argv[1]));
	if (SOCKET_ERROR == bind(srvSocket, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		closesocket(srvSocket);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(srvSocket, 5))
	{
		closesocket(srvSocket);
		WSACleanup();
		return -1;
	}

	SOCKET cltSocket;
	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int nCltAddrSize = sizeof(cltAddr);

	int strLen = 0;
	char msg[BUF_SIZE];

	for (int i = 0; i < 5; i++)
	{
		cltSocket = accept(srvSocket, (sockaddr*)&cltAddr, &nCltAddrSize);
		if (cltSocket == INVALID_SOCKET)
		{
			break;
		}
		else
		{
			printf("Connected client %d \n", i + 1);

			//inet_ntoa会自动将ULONG转成字符串类型，并且还会自动进行网络字节顺序到主机字节顺序的转换
			printf("Current Client IP: %s, Port: %d", inet_ntoa(cltAddr.sin_addr), ntohs(cltAddr.sin_port));
		}

		while ( (strLen = recv(cltSocket, msg, BUF_SIZE, 0)) != 0 )
		{
			printf("recv msg len from client: %d \n", strLen);
			printf("recv msg form client: %s \n", msg);
			
			//回传
			send(cltSocket, msg, strLen, 0);
		}

		closesocket(cltSocket);
	}

	closesocket(srvSocket);
	WSACleanup();

	return 0;
}

