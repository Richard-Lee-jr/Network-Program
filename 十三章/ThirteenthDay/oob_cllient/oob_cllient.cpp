// oob_cllient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")



int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		printf("argc error!");
		return -1;
	}

	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("WSAStartup error!");
		return -1;
	}

	SOCKET cltSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == cltSock)
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

	if (SOCKET_ERROR == connect(cltSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		printf("connect error!");
		closesocket(cltSock);
		WSACleanup();
		return -1;
	}

	send(cltSock, "123", 3, 0);
	send(cltSock, "4", 1, MSG_OOB);
	send(cltSock, "567", 3, 0);
	send(cltSock, "890", 3, MSG_OOB);

	closesocket(cltSock);
	WSACleanup();

	puts("任意键继续...");
	getchar();

	return 0;
}

