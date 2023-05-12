// HalfClose_Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 30

/*
Windows��رսӿ�
int shutdow(SOCKET sock, int howto);
sock: Ҫ��رյ��׽��־��
howto: �Ͽ���ʽ����Ϣ.��ѡ��SD_RECEIVE -- �Ͽ���������SD_SEND -- �Ͽ��������SD_BOTH -- ͬʱ�Ͽ�IO��
����ֵ���ɹ�ʱ����0��ʧ��ʱ����SOCKET_ERROR
*/

int _tmain(int argc, _TCHAR* argv[])
{


	if (argc != 2)
	{
		return -1;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup Error!");
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
	int nCltAddrSize = sizeof(cltAddr);

	SOCKET cltSock = accept(srvSock, (sockaddr*)&cltAddr, &nCltAddrSize);
	if (INVALID_SOCKET == cltSock)
	{
		printf("accept error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	int nRecvLen = 0;
	char Msg[BUF_SIZE] = "0123456789";
	while(true)
	{
		nRecvLen = strlen(Msg);
		printf("���ճ���: %d\n", nRecvLen);

		if (nRecvLen < BUF_SIZE)
		{
			printf("break\n");
			send(cltSock, Msg, nRecvLen, 0);
			break;
		}

		send(cltSock, Msg, BUF_SIZE, 0);
	}

	//��رգ������
	shutdown(cltSock, SD_SEND);

	nRecvLen = recv(cltSock, Msg, BUF_SIZE, 0);
	printf("Msg From Client: %s\n", Msg);

	closesocket(srvSock);
	WSACleanup();

	fputs("���������...", stdout);
	getchar();
	return 0;
}

