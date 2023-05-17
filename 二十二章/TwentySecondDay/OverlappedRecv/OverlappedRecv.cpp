// OverlappedRecv.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")


#define BUF_SIZE 1024

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		puts("argc error!");
		return -1;
	}

	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		puts("WSAStartup error!");
		return -1;
	}

	//ʹ��WSASocket�������������ص�IO���׽���
	SOCKET srvSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == srvSock)
	{
		puts("WSASocket error!");
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
		puts("bind error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(srvSock, 5))
	{
		puts("listen error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN cltAddr;
	int nCltAddrSize = sizeof(cltAddr);
	memset(&cltAddr, 0, nCltAddrSize);
	
	SOCKET cltSock = accept(srvSock, (sockaddr*)&cltAddr, &nCltAddrSize);


	WSAEVENT overlappedEvent = WSACreateEvent();
	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = overlappedEvent;

	char buf[BUF_SIZE] = {};
	WSABUF dataBuf;
	dataBuf.buf = buf;
	dataBuf.len = BUF_SIZE;

	int nRecvLen = 0, flags = 0;
	if (SOCKET_ERROR == WSARecv(cltSock, &dataBuf, 1, (LPDWORD)&nRecvLen, (LPDWORD)&flags, &overlapped, NULL))
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			//��̨���ڽ�������
			puts("Background data receive");

			//���������ȴ����ݴ������
			WSAWaitForMultipleEvents(1, &overlappedEvent, TRUE, WSA_INFINITE, FALSE);

			//�õ�����������Ϣ
			WSAGetOverlappedResult(cltSock, &overlapped, (LPDWORD)&nRecvLen, FALSE, NULL);
		}
		else
		{
			puts("WSARecv error!");
		}
	}

	//���WSARecv������SOCKET_ERROR����˵����������������
	//dataBuf.buf[nRecvLen] = 0;
	printf("Received len: %d, message: %s \n", nRecvLen, dataBuf.buf);

	closesocket(cltSock);
	closesocket(srvSock);
	WSACloseEvent(overlappedEvent);
	WSACleanup();

	puts("���������...");
	getchar();

	return 0;
}

