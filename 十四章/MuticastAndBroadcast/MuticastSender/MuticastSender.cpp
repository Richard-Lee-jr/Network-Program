// MuticastSender.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define TTL 64
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

	SOCKET senderSock = socket(PF_INET, SOCK_DGRAM, 0);
	if (senderSock == INVALID_SOCKET)
	{
		printf("socket error!");
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN senderAddr;
	memset(&senderAddr, 0, sizeof(senderAddr));
	senderAddr.sin_family = PF_INET;
	senderAddr.sin_addr.s_addr = inet_addr(argv[1]);
	senderAddr.sin_port = htons(_ttoi(argv[2]));

	int timeLive = TTL;
	//IP_MULTICAST_TTL 需 #include <WS2tcpip.h>
	if (SOCKET_ERROR == setsockopt(senderSock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&timeLive, sizeof(timeLive)) )
	{
		printf("setsockopt error!");
		closesocket(senderSock);
		WSACleanup();
		return -1;
	}

	FILE* fp;
	fp = fopen("news.txt", "r");
	if (fp == NULL)
	{
		printf("setsockopt error!");
		closesocket(senderSock);
		WSACleanup();
		return -1;
	}

	char Msg[BUF_SIZE] = {};
	while(!feof(fp))
	{
		fgets(Msg, BUF_SIZE, fp);
		sendto(senderSock, Msg, strlen(Msg), 0, (sockaddr*)&senderAddr, sizeof(senderAddr));
		Sleep(2000);
	}


	closesocket(senderSock);
	WSACleanup();


	puts("任意键继续...");
	getchar();


	return 0;
}

