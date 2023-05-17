// OverlappedSend.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")


#define BUF_SIZE 1024


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
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
	SOCKET cltSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == cltSock)
	{
		puts("WSASocket error!");
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	if (SOCKET_ERROR == connect(cltSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		puts("connect error!");
		closesocket(cltSock);
		WSACleanup();
		return -1;
	}

	char buf[] = "Network is Computer";

	//重叠IO模型准备
	WSAEVENT overlappedEvent = WSACreateEvent();
	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(WSAOVERLAPPED));
	overlapped.hEvent = overlappedEvent;

	WSABUF dataBuf;
	dataBuf.buf = buf;
	dataBuf.len = strlen(buf) + 1;

	int nTmp = sizeof(buf);

	int nSendLen = 0, flags = 0;
	if (SOCKET_ERROR == WSASend(cltSock, &dataBuf, 1, (LPDWORD)&nSendLen, 0, &overlapped, NULL))
	{
		if (GetLastError() == WSA_IO_PENDING)
		{
			puts("Background data send.");

			//等待数据发送完成
			WSAWaitForMultipleEvents(1, &overlappedEvent, TRUE, WSA_INFINITE, FALSE);

			//获取发送数据信息
			WSAGetOverlappedResult(cltSock, &overlapped, (LPDWORD)&nSendLen, FALSE, NULL);
		}
		else
		{
			puts("WSASend error");
		}
	}

	printf("send data size: %d \n", nSendLen);
	WSACloseEvent(overlappedEvent);
	closesocket(cltSock);
	WSACleanup();

	puts("任意键继续...");
	getchar();

	return 0;
}

