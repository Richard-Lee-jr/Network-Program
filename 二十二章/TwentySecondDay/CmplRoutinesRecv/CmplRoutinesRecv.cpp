// CmplRoutinesRecv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")


#define BUF_SIZE 1024

void CALLBACK CpmpRoutine(DWORD , DWORD , LPWSAOVERLAPPED , DWORD);

WSABUF dataBuf;
char buf[BUF_SIZE];
int nRecvLen = 0;

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

	//使用WSASocket函数创建用于重叠IO的套接字
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

	//使用CompletionRoutine函数代替事件
	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));

	dataBuf.buf = buf;
	dataBuf.len = BUF_SIZE;

	WSAEVENT waitEvent = WSACreateEvent();

	int flags = 0;

	//接收数据，接收完成时，判断线程是否在alertable wait状态，在则调用CpmpRoutine
	if (SOCKET_ERROR == WSARecv(cltSock, &dataBuf, 1, (LPDWORD)&nRecvLen, (LPDWORD)&flags, &overlapped, CpmpRoutine))
	{
		if (GetLastError() == WSA_IO_PENDING)
		{
			//后台数据正在接收
			puts("Background data receive");

			
		}
		else
		{
			puts("WSARecv error!");
			closesocket(cltSock);
			closesocket(srvSock);
			WSACloseEvent(waitEvent);
			WSACleanup();
			return -1;
		}
	}

	//nRecvLen = szRecvBytes;
	//printf("Receive len: %d, message: %s\n", nRecvLen, dataBuf.buf);

	//进入alertable wait状态，等待数据接收完成
	int idx = WSAWaitForMultipleEvents(1, &waitEvent, FALSE, WSA_INFINITE, TRUE);

	if (idx == WSA_WAIT_IO_COMPLETION)
	{
		puts("Overlapped IO Completed!");
	}
	else
	{
		puts("WSAWaitForMultipleEvents error!");
	}

	closesocket(cltSock);
	closesocket(srvSock);
	WSACloseEvent(waitEvent);
	WSACleanup();

	puts("任意键继续...");
	getchar();

	return 0;
}

void CALLBACK CpmpRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	if (dwError != 0)
	{
		puts("CompRoutine error!");
	}
	else
	{
		nRecvLen = szRecvBytes;
		printf("CpmpRoutine--Receive len: %d, message: %s\n", nRecvLen, dataBuf.buf);
	}
}