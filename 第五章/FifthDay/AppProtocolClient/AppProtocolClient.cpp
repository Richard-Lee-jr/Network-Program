// AppProtocolClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024
#define OPSZ 4
#define RLT_SIZE 4

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
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
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));
	int srvAddrLen = sizeof(srvAddr);

	if (SOCKET_ERROR == connect(srvSock, (sockaddr*)&srvAddr, srvAddrLen))
	{
		printf("connect error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	printf("Connected...");

	int opnd_cnt = 0;
	fputs("����������: ", stdout);
	scanf("%d", &opnd_cnt);

	char Msg[BUF_SIZE];
	Msg[0] = (char)opnd_cnt;

	for (int i = 0; i < opnd_cnt; i++)
	{
		printf("������: ", stdout);
		scanf("%d", (int*)&Msg[i * OPSZ + 1]);
	}

	//������뻺��
	fgetc(stdin);

	fputs("������: ", stdout);
	scanf("%c", &Msg[opnd_cnt * OPSZ + 1]);

	printf("Send to Server: %c\n", Msg[opnd_cnt * OPSZ + 1]);
	send(srvSock, Msg, opnd_cnt * OPSZ + 2, 0);
	
	int result = 0;
	recv(srvSock, (char*)&result, RLT_SIZE, 0);

	printf("�������: %d\n", result);
	closesocket(srvSock);
	WSACleanup();

	getchar();
	getchar();

	return 0;
}

