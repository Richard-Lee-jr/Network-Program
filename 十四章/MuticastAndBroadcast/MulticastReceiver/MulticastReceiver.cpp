// MulticastReceiver.cpp : �������̨Ӧ�ó������ڵ㡣
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
	groupAddr.imr_multiaddr.s_addr = inet_addr(argv[1]);	//��ʼ���ಥ���ַ
	groupAddr.imr_interface.s_addr = htons(ADDR_ANY);		//��ʼ��������������IP��ַ

	//�����׽���ѡ��IP_ADD_MEMBERSHIP����ಥ�顣��������˽��յ�53��ָ���Ķಥ�����ݵ�����׼��
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

