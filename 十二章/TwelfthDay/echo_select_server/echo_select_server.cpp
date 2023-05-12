// echo_select_server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "select.h"

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024

int _tmain(int argc, _TCHAR* argv[])
{
	
	if (argc != 2)
	{
		printf("argc error!\n");
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
		printf("socket error!\n");
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = htonl(ADDR_ANY);
	srvAddr.sin_port = htons(_ttoi(argv[1]));

	if (SOCKET_ERROR == bind(srvSock, (sockaddr*)&srvAddr, sizeof(srvAddr)))
	{
		printf("bind error!\n");
		return -1;
	}

	if (SOCKET_ERROR == listen(srvSock, 5))
	{
		printf("listen error!\n");
		return -1;
	}

	fd_set reads, temps;
	FD_ZERO(&reads);
	FD_SET(srvSock, &reads);

	timeval timeout;

	int nFDNum;

	SOCKET cltSock;
	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int nCltAddrLen = 0;

	int nRecvLen = 0;
	char Msg[BUF_SIZE];

	while (true)
	{
		temps = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((nFDNum = select(0, &temps, nullptr, nullptr, &timeout)) == SOCKET_ERROR)
		{
			printf("select error!\n");
			break;
		}

		printf("nFDNum: %d \n", nFDNum);

		if (nFDNum == 0)
		{
			printf("select time out!\n");
			continue;
		}

		for (int i = 0; i < temps.fd_count; i++)
		{
			if (FD_ISSET(temps.fd_array[i], &temps))
			{
				if (temps.fd_array[i] == srvSock)
				{
					cltSock = accept(srvSock, (sockaddr*)&cltAddr, &nCltAddrLen);
					if (INVALID_SOCKET == cltSock)
					{
						printf("accept error!\n");
						continue;
					}
					FD_SET(cltSock, &reads);
					printf("connected client: %d \n", cltSock);
				}
				else
				{
					//读取客户端发来信息
					nRecvLen = recv(temps.fd_array[i], Msg, BUF_SIZE, 0);
					if (nRecvLen == 0)
					{
						//断开连接
						FD_CLR(temps.fd_array[i], &reads);
						closesocket(temps.fd_array[i]);
						printf("closed client: %d \n", temps.fd_array[i]);
					}
					else
					{
						//回发
						send(temps.fd_array[i], Msg, nRecvLen, 0);
					}
				}
			}
		}
	}

	closesocket(srvSock);
	WSACleanup();
	return 0;
}

