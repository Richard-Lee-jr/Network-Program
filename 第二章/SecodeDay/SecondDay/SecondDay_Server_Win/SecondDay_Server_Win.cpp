// SecondDay_Server_Win.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

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

	SOCKET srvSocket = socket(PF_INET, SOCK_STREAM, 0);
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

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(SOCKADDR_IN));
	int nCltAddrSize = sizeof(cltAddr);
	SOCKET cltSocket = accept(srvSocket, (sockaddr*)&cltAddr, &nCltAddrSize);
	if (INVALID_SOCKET == cltSocket)
	{
		closesocket(srvSocket);
		WSACleanup();
		return -1;
	}

	char msg[] = "hello world!";

	send(cltSocket, msg, sizeof(msg), 0);

	send(cltSocket, "hello tcp", sizeof("hello tcp"), 0);

	send(cltSocket, "hello network", sizeof("hello network"), 0);
	


	closesocket(cltSocket);
	closesocket(srvSocket);
	WSACleanup();

	getchar();

	return 0;
}