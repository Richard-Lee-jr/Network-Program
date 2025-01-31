// UDP_Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
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
		printf("WSAStartup Error!\n");
		return -1;
	}

	SOCKET srvSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == srvSock)
	{
		printf("socket Error!\n");
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = htonl(ADDR_ANY);
	srvAddr.sin_port = htons(_ttoi(argv[1]));
	int srvAddrLen = sizeof(srvAddr);

	if (SOCKET_ERROR == bind(srvSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
	printf("bind Error!\n");
	closesocket(srvSock);
	WSACleanup();
	return -1;
	}

	char Msg[BUF_SIZE];
	int recvLen = 0;

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int nCltAddrLen = sizeof(cltAddr);

	//for (int i = 0; i < 3; i++)
	//{
	//	recvLen = 0;
	//	recvLen = recvfrom(srvSock, Msg, BUF_SIZE - 1, 0, (sockaddr*)&cltAddr, &nCltAddrLen);
	//	Msg[recvLen] = 0;
	//	printf("recvfrom client msg: %s\n", Msg);
	//}
	


	while (true)
	{
		printf("Wait Msg From Client...\n");
		//无连接的UDP套接字，sendto和recvfrom必须一一对应，也没有数据边界
		//无连接的UDP套接字不会保持连接状态，因此每次传输数据都需要添加目标地址信息。
		recvLen = recvfrom(srvSock, Msg, BUF_SIZE - 1, 0, (sockaddr*)&cltAddr, &nCltAddrLen);
		printf("接收长度: %d, 最后一个字符: %d\n", recvLen, Msg[recvLen]);
		if (recvLen == -1)
		{
			//UDP属于无连接协议，因此没有断开连接的说法，这里不会进来
			printf("Client Disconnected.");
			break;
		}

		Msg[recvLen] = 0;
		printf("Receive Msg from Client: %s\n", Msg);

		sendto(srvSock, Msg, recvLen, 0, (sockaddr*)&cltAddr, nCltAddrLen);
	}

	closesocket(srvSock);
	WSACleanup();

	getchar();

	return 0;
}

