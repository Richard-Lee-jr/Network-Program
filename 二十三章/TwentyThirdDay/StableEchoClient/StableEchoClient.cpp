// StableEchoClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		return -1;
	}

	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return -1;
	}

	SOCKET cltSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == cltSock)
	{
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	if (SOCKET_ERROR == connect(cltSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		return -1;
	}

	char msg[BUF_SIZE] = {};

	int nSendLen = 0, nRecvLen = 0;
	while (true)
	{
		fputs("Input msg(Q to quit): ", stdout);
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			break;
		}

		nSendLen = strlen(msg);
		send(cltSock, msg, nSendLen, 0);

		nRecvLen = 0;
		while (true)
		{
			nRecvLen += recv(cltSock, msg, BUF_SIZE - 1 - nRecvLen, 0);
			//确保接收长度>=发送长度
			if (nRecvLen >= nSendLen)
			{
				break;
			}
		}

		msg[nRecvLen] = 0;
		printf("msg from server: %s\n", msg);

	}

	closesocket(cltSock);
	WSACleanup();

	return 0;
}

