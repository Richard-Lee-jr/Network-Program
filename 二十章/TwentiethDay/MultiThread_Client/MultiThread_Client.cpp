// MultiThread_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <process.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void* arg);
unsigned WINAPI RecvMsg(void* arg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE] = {};


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 4)
	{
		printf("argc error!\n");
		return -1;
	}

	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("WSAStartup error!\n");
		return -1;
	}

	printf("server ip: %s, port: %s, client name: %s\n", argv[1], argv[2], argv[3]);
	sprintf(name, "[%s]", argv[3]);

	SOCKET cltSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == cltSock)
	{
		puts("socket error!");
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	if (connect(cltSock, (sockaddr*)&srvAddr, sizeof(srvAddr)) == SOCKET_ERROR)
	{
		puts("connect error!");
		closesocket(cltSock);
		WSACleanup();
		return -1;
	}

	//开启客户端的接收和发送线程
	HANDLE hSendThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&cltSock, 0, NULL);
	HANDLE hRecvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&cltSock, 0, NULL);

	WaitForSingleObject(hSendThread, INFINITE);
	WaitForSingleObject(hRecvThread, INFINITE);

	closesocket(cltSock);
	WSACleanup();

	puts("任意键继续...");
	getchar();

	return 0;
}

unsigned WINAPI SendMsg(void* arg)
{
	SOCKET cltSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE] = {};
	while (true)
	{
		//printf("Input Msg: ");
		fgets(msg, BUF_SIZE, stdin);

		if ( !strcmp(msg, "q\n") || !strcmp(msg, "Q\n") )
		{
			puts("Disconnect...");
			break;
		}

		sprintf(nameMsg, "%s %s", name, msg);
		send(cltSock, nameMsg, strlen(nameMsg), 0);
	}
	//exit(0);
	closesocket(cltSock);

	printf("client %d thread end.\n", cltSock);
	return 0;
}

unsigned WINAPI RecvMsg(void* arg)
{
	SOCKET cltSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE] = {};

	int nRecvLen = 0;
	while (true)
	{
		nRecvLen = recv(cltSock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (nRecvLen == -1)
		{
			puts("server disconnected!");
			return -1;
		}
		nameMsg[nRecvLen] = 0;
		printf("nameMsg from server: %s\n", nameMsg);
	}

	return 0;
}