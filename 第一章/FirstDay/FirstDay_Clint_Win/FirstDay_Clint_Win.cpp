// FirstDay_Clint_Win.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>

void ErrorHandle(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

int _tmain(int argc, char* argv[])
{
	if (argc != 3)
	{
		ErrorHandle("argc error!");
		return -1;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandle("WSAStartup error!");
		return -1;
	}

	SOCKET clt_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == clt_socket)
	{
		WSACleanup();
		ErrorHandle("socket error!");
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(SOCKADDR_IN));
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	if (INVALID_SOCKET == connect(clt_socket, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		closesocket(clt_socket);
		WSACleanup();
		ErrorHandle("connect error!");
		return -1;
	}

	char msg[30];
	int nRevLen = recv(clt_socket, msg, sizeof(msg), 0);
	if (nRevLen == -1)
	{
		closesocket(clt_socket);
		WSACleanup();
		ErrorHandle("recv error!");
		return -1;
	}

	printf("msg form server: %s\n", msg);

	closesocket(clt_socket);
	WSACleanup();

	getchar();
	return 0;
}

