// echo_server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024

int _tmain(int argc, _TCHAR* argv[])
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

	SOCKET srvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srvSock)
	{
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));

	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = htonl(ADDR_ANY);
	srvAddr.sin_port = htons(_ttoi(argv[1]));
	if (SOCKET_ERROR == bind(srvSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(srvSock, 5))
	{
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int cltAddrLen = sizeof(cltAddr);

	int strLen = 0;
	char Msg[BUF_SIZE];
	for (int i = 0; i < 5; i++)
	{
		printf("Wait Client Connect...\n");
		SOCKET cltSock = accept(srvSock, (sockaddr*)&cltAddr, &cltAddrLen);
		if (cltSock == INVALID_SOCKET)
		{
			printf("accept error!");
			break;
		}
		else
		{
			printf("Connected client %d \n", i + 1);
		}

		while ( (strLen = recv(cltSock, Msg, BUF_SIZE, 0)) != 0)
		{
			send(cltSock, Msg, strLen, 0);
		}

		closesocket(cltSock);
	}

	closesocket(srvSock);
	WSACleanup();
	return 0;
}

