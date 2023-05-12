// UDP_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		return -1;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup Error!\n");
		return -1;
	}

	SOCKET cltSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == cltSock)
	{
		printf("socket Error!\n");
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvAddr.sin_port = htons(_ttoi(argv[2]));

	int nSrvAddrLen = sizeof(srvAddr);

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int nCltAddrLen = sizeof(cltAddr);

	//将UDP套接字转换成连接套接字
	connect(cltSock, (sockaddr*)&srvAddr, sizeof(srvAddr));


	char Msg[BUF_SIZE] = "0123456789";
	int recvLen = 0;

	//客户端发送三次数据，服务器端调用一次recvfrom试图接收三次数据。是行不通的
	//int nSendLend = 0;
	//nSendLend += sendto(cltSock, Msg, strlen(Msg), 0, (sockaddr*)&srvAddr, nSrvAddrLen);
	//nSendLend += sendto(cltSock, Msg, strlen(Msg), 0, (sockaddr*)&srvAddr, nSrvAddrLen);
	//nSendLend += sendto(cltSock, Msg, strlen(Msg), 0, (sockaddr*)&srvAddr, nSrvAddrLen);


	while (true)
	{
		fputs("Input Msg(q to quit): ", stdout);
		fgets(Msg, sizeof(Msg), stdin);
		
		if (!strcmp(Msg, "q\n") || !strcmp(Msg, "Q\n"))
		{
			printf("Disconnected...");
			break;
		}

		//sendto时发现尚未分配地址信息，则在首次调用sendto函数时给相应套接字自动分配IP和端口
		//通过sendto函数传输数据的过程大致可分为以下3个阶段
		//①向UDP套接字注册目标IP和端口号
		//②传输数据
		//③删除UDP套接字中注册的目标地址信息

		//每次调用sendto函数时重复上述过程。每次变更目标地址，因此可以重复利用同一UDP套接字向不同目标传输数据。
		//这种事先注册目标地址信息，在sendto时才注册的套接字称为未连接套接字。反之事先注册了目标地址的套接字称为连接套接字。显然UDP套接字默认属于未连接套接字。
		//但UDP套接字在只需向一个目标地址传输数据时就显得不太合理
		//例：IP为169.21.32.110的主机9190号端口共准备了3个数据，因此需要调用三次sendto函数进行传输。
		//此时需要重复上述三阶段，上述三个阶段中，第①、③个阶段占整个通信过程近1/3的时间，缩短这部分时间将大大提高整体效率。
		//因此，要与同一主机进行长时间通信，将UDP套接字编程连接套接字会提供效率。
		//如何将UDP套接字变成连接套接字？使用connect函数实现。
		sendto(cltSock, Msg, strlen(Msg), 0, (sockaddr*)&srvAddr, sizeof(srvAddr));

		recvLen = recvfrom(cltSock, Msg, BUF_SIZE - 1, 0, (sockaddr*)&srvAddr, &nSrvAddrLen);
		Msg[recvLen] = 0;

		//已连接的UDP套接字可以直接使用send 和 recv函数
		//send(cltSock, Msg, strlen(Msg), 0);

		//recvLen = recv(cltSock, Msg, BUF_SIZE - 1, 0);
		//Msg[recvLen] = 0;

		printf("Msg From Server: %s\n", Msg);
	}
	

	closesocket(cltSock);
	WSACleanup();

	getchar();

	return 0;
}

