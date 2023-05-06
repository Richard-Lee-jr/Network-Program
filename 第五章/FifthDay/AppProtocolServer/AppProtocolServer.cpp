// AppProtocolServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024
#define OPSZ 4

int calculate(int opnum, int opnds[], char op)
{
	int result = opnds[0], i;

	printf("操作数数量: %d, 第一个操作数: %d, 操作符: %c\n", opnum, result, op);

	switch(op)
	{
	case '+':
		for (i = 1; i < opnum; i++)
		{
			result += opnds[i];
		}
		break;

	case '-':
		for (i = 1; i < opnum; i++)
		{
			result -= opnds[i];
		}
		break;

	case '*':
		for (i = 1; i < opnum; i++)
		{
			result *= opnds[i];
		}
		break;

	default:
		for (i = 1; i < opnum; i++)
		{
			result += opnds[i];
		}
		break;
	}

	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		printf("arg error!");
		return -1;
	}

	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
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
	srvAddr.sin_addr.s_addr = htonl(ADDR_ANY);
	srvAddr.sin_port = htons(_ttoi(argv[1]));

	if (SOCKET_ERROR == bind(srvSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		printf("bind error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}
	
	if (SOCKET_ERROR == listen(srvSock, 5))
	{
		printf("listen error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int cltAddrLen = sizeof(cltAddr);

	int opnd_cnt = 0;

	int recvLen = 0;
	char Msg[BUF_SIZE];
	int result = 0;
	for (int i = 0; i < 5; i++)
	{
		printf("Wait client Connect...\n");
		opnd_cnt = 0;
		SOCKET cltSock = accept(srvSock, (sockaddr*)&cltAddr, &cltAddrLen);
		if (INVALID_SOCKET == cltSock)
		{
			printf("accept error!");
			closesocket(srvSock);
			WSACleanup();
			return -1;
		}

		printf("Client %d Connected.\n", i + 1);

		//得到操作数量
		recv(cltSock, (char*)&opnd_cnt, 1, 0);

		recvLen = 0;
		while ( (opnd_cnt * OPSZ + 1) > recvLen )
		{
			int recv_cnt = recv(cltSock, &Msg[recvLen], BUF_SIZE - 1, 0);
			recvLen += recv_cnt;
		}
		
		printf("接收长度: %d, 操作符: %c\n", recvLen, Msg[recvLen - 1]);
		
		//printf("Msg: %s\n", Msg);
		result  = calculate(opnd_cnt, (int*)Msg, Msg[recvLen - 1]);
		send(cltSock, (char*)&result, sizeof(result), 0);

		closesocket(cltSock);
	}

	closesocket(srvSock);
	WSACleanup();

	return 0;
}

