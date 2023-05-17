// MulticastReceiver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 30



int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		printf("argc error!");
		return -1;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup error!");
		return -1;
	}

	SOCKET recvSock = socket(PF_INET, SOCK_DGRAM, 0);
	if (recvSock == INVALID_SOCKET)
	{
		printf("socket error!");
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN recvAddr;
	memset(&recvAddr, 0, sizeof(recvAddr));
	recvAddr.sin_family = PF_INET;
	recvAddr.sin_addr.s_addr = htonl(ADDR_ANY);	//
	recvAddr.sin_port = htons(_ttoi(argv[2]));

	if (SOCKET_ERROR == bind(recvSock, (sockaddr*)&recvAddr, sizeof(recvAddr)))
	{
		printf("socket error!");
		closesocket(recvSock);
		WSACleanup();
		return -1;
	}

	IP_MREQ groupAddr;
	memset(&groupAddr, 0, sizeof(groupAddr));
	groupAddr.imr_multiaddr.s_addr = inet_addr(argv[1]);	//初始化多播组地址
	groupAddr.imr_interface.s_addr = htons(ADDR_ANY);		//初始化待加入主机的IP地址

	//利用套接字选项IP_ADD_MEMBERSHIP加入多播组。至此完成了接收第53行指定的多播组数据的所有准备
	if (SOCKET_ERROR == setsockopt(recvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&groupAddr, sizeof(groupAddr)))
	{
		printf("setsockopt error!");
		closesocket(recvSock);
		WSACleanup();
		return -1;
	}

	char Msg[BUF_SIZE] = {};
	int nRecvLen = 0;
	while (true)
	{
		nRecvLen = recvfrom(recvSock, Msg, BUF_SIZE - 1, 0, NULL, 0);
		if (nRecvLen < 0)
		{
			printf("disconnect...\n");
			break;
		}
		Msg[nRecvLen] = 0;
		fputs(Msg, stdout);
	}
	closesocket(recvSock);
	WSACleanup();

	return 0;
}

