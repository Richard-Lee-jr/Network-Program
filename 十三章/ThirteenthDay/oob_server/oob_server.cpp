// oob_server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")


#define BUF_SIZE 1024

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		printf("argc error!");
		return -1;
	}

	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("WSAStartup error!");
		return -1;
	}

	SOCKET srvScok = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srvScok)
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

	if (SOCKET_ERROR == bind(srvScok, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		printf("bind error!");
		closesocket(srvScok);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(srvScok, 5))
	{
		printf("listen error!");
		closesocket(srvScok);
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int nCltAddrSize = sizeof(cltAddr);

	fd_set srvReads, cltExcept;
	FD_ZERO(&srvReads);
	FD_ZERO(&cltExcept);
	FD_SET(srvScok, &srvReads);

	int nRecvLen = 0;
	char Msg[BUF_SIZE] = {};

	fd_set cpySrvReads, cpyCltExcept;
	FD_ZERO(&cpySrvReads);
	FD_ZERO(&cpyCltExcept);
	int nSelRet = 0;

	TIMEVAL timeout;
	

	while (true)
	{
		cpySrvReads = srvReads;
		cpyCltExcept = cltExcept;

		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		nSelRet = select(0, &cpySrvReads, NULL, &cpyCltExcept, &timeout);
		if (nSelRet == -1)
		{
			printf("select error!");
			break;
		}

		if (nSelRet == 0)
		{
			printf("time out!\n");
			continue;
		}

		for (int i = 0; i < cpyCltExcept.fd_count; i++)
		{
			if (FD_ISSET(cpyCltExcept.fd_array[i], &cltExcept))
			{
				nRecvLen = recv(cpyCltExcept.fd_array[i], Msg, BUF_SIZE - 1, MSG_OOB);
				Msg[nRecvLen] = 0;
				printf("Urgent message: %s\n", Msg);
			}
		}

		for (int i = 0; i < cpySrvReads.fd_count; i++)
		{
			if (cpySrvReads.fd_array[i] == srvScok)
			{
				SOCKET cltSock = accept(srvScok, (sockaddr*)&cltAddr, &nCltAddrSize);
				if (cltSock == INVALID_SOCKET)
				{
					printf("accept error!");
					continue;
				}

				FD_SET(cltSock, &srvReads);
				FD_SET(cltSock, &cltExcept);
				printf("client %d connected...\n", cltSock);
			}
			else
			{
				//有读取消息
				nRecvLen = recv(cpySrvReads.fd_array[i], Msg, BUF_SIZE - 1, 0);
				if (nRecvLen == 0)
				{
					printf("client %d disconnect...\n", cpySrvReads.fd_array[i]);
					FD_CLR(cpySrvReads.fd_array[i], &srvReads);
					FD_CLR(cpySrvReads.fd_array[i], &cltExcept);
					closesocket(cpySrvReads.fd_array[i]);
				}
				else
				{
					Msg[nRecvLen] = 0;
					printf("Msg From Client: %s\n", Msg);
				}
			}
		}
	}

	/*
	SOCKET cltSock = accept(srvScok, (sockaddr*)&cltAddr, &nCltAddrSize);
	if (INVALID_SOCKET == cltSock)
	{
		printf("accept error!");
		closesocket(srvScok);
		WSACleanup();
		return -1;
	}
	printf("client %d connected...\n", cltSock);

	fd_set reads, except;
	FD_ZERO(&reads);
	FD_ZERO(&except);
	FD_SET(cltSock, &reads);
	FD_SET(cltSock, &except);

	fd_set cpyReads, cpyExcept;
	TIMEVAL timeout;

	int nSelRet = 0;

	int nRecvLen = 0;
	char Msg[BUF_SIZE] = {0};
	while (true)
	{
		cpyReads = reads;
		cpyExcept = except;

		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		nSelRet = select(0, &cpyReads, NULL, &cpyExcept, &timeout);
		if (nSelRet == -1)
		{
			printf("select error!");
			break;
		}

		if (nSelRet == 0)
		{
			printf("time out\n");
			continue;
		}

		if (FD_ISSET(cltSock, &cpyExcept))
		{
			nRecvLen = recv(cltSock, Msg, BUF_SIZE - 1, MSG_OOB);
			Msg[nRecvLen] = 0;
			printf("Urgent message: %s\n", Msg);
		}

		if (FD_ISSET(cltSock, &cpyReads))
		{
			nRecvLen = recv(cltSock, Msg, BUF_SIZE - 1, 0);

			if (nRecvLen == 0)
			{
				printf("client disconnect...\n");
				closesocket(cltSock);
				break;
			}
			else
			{
				Msg[nRecvLen] = 0;
				printf("Normal Msg: %s\n", Msg);
			}
			
		}
	}
	*/

	closesocket(srvScok);
	WSACleanup();

	puts("任意键继续...");
	getchar();

	return 0;
}
