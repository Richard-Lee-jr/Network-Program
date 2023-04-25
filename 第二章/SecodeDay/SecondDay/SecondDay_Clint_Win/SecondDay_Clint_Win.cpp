// SecondDay_Clint_Win.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>

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

	SOCKET srvSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srvSocket)
	{
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(SOCKADDR_IN));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	if (SOCKET_ERROR == connect(srvSocket, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		closesocket(srvSocket);
		WSACleanup();
		return -1;
	}


	//体现：tcp传输可靠的、按序传输、基于字节的面向连接的特性
	//服务器一次send，客户端多次读取
	//int nRevLen = 0;
	//char revMsg[30];
	//int nIndex = 0;
	//int nStrLen = 0;
	//while (nRevLen = recv(srvSocket, &revMsg[nIndex++], 1, 0))
	//{
	//	if (nRevLen == -1)
	//	{
	//		closesocket(srvSocket);
	//		WSACleanup();
	//		return -1;
	//	}

	//	nStrLen += nRevLen;
	//	if (revMsg[nIndex - 1] == '\0')
	//	{
	//		break;
	//	}
	//}

	//std::cout << "msg from server: " << revMsg << std::endl;
	//std::cout << "strLen: " << nStrLen << std::endl;

	for (int i = 0; i < 3000; i++)
	{
		std::cout << "wait time: " << i << std::endl;
	}

	char msg[100];
	int nRevLen = recv(srvSocket, msg, sizeof(msg), 0);

	//std::cout << "msg from server: " << msg << std::endl;
	printf("msg from server: %s", msg);

	closesocket(srvSocket);
	WSACleanup();

	getchar();
	return 0;
}

