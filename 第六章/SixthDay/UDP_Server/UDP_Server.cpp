// UDP_Server.cpp : �������̨Ӧ�ó������ڵ㡣
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
	while (true)
	{
		printf("Wait Msg From Client...\n");
		//�����ӵ�UDP�׽��֣�sendto��recvfrom����һһ��Ӧ��Ҳû�����ݱ߽�
		//�����ӵ�UDP�׽��ֲ��ᱣ������״̬�����ÿ�δ������ݶ���Ҫ���Ŀ���ַ��Ϣ��
		recvLen = recvfrom(srvSock, Msg, BUF_SIZE - 1, 0, (sockaddr*)&srvAddr, &srvAddrLen);
		if (recvLen == -1)
		{
			printf("Client Disconnected.");
			break;
		}
		Msg[recvLen] = 0;
		printf("Receive Msg from Client: %s\n", Msg);

		sendto(srvSock, Msg, recvLen, 0, (sockaddr*)&srvAddr, srvAddrLen);
	}
	closesocket(srvSock);
	WSACleanup();

	return 0;
}

