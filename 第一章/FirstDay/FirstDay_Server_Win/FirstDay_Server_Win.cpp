// FirstDay_Server_Win.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <WinSock2.h>

void ErrorHandle(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

int _tmain(int argc, char* argv[])
{
	if (argc != 2)
	{
		ErrorHandle("argc error!");
		return -1;
	}

	//��ӡ�����ip �� �˿�
	printf("Port: %s \n", argv[1]);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandle("WSAStartup Error!");
		return -1;
	}

	//�����׽��֣���װ�绰��
	SOCKET srv_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (srv_socket == INVALID_SOCKET)
	{
		WSACleanup();
		ErrorHandle("create socket error!");
		return -1;
	}

	//��IP��Port������绰���룩
	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(SOCKADDR_IN));
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvAddr.sin_port = htons(_ttoi(argv[1]));

	if (SOCKET_ERROR == bind(srv_socket, (sockaddr*)&srvAddr, sizeof(SOCKADDR_IN)))
	{
		closesocket(srv_socket);
		WSACleanup();

		ErrorHandle("bind error!");
		
		return -1;
	}

	//���������ӵ绰�ߣ�
	if (SOCKET_ERROR == listen(srv_socket, 5))
	{
		closesocket(srv_socket);
		WSACleanup();

		ErrorHandle("listen error!");
		
		return -1;
	}

	//�������ӣ�����绰������
	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(SOCKADDR_IN));
	int nCltAddrSize = sizeof(cltAddr);
	SOCKET clt_socket = accept(srv_socket, (sockaddr*)&cltAddr, &nCltAddrSize);
	if (INVALID_SOCKET == clt_socket)
	{
		closesocket(srv_socket);
		WSACleanup();

		ErrorHandle("accept error!");
		
		return -1;
	}

	//����ɴ�ӡ���ӵĿͻ��˵ĵ�ַ��Ϣ


	//IO��������ͨ��
	char msg[] = "hello world!";
	send(clt_socket, msg, sizeof(msg), 0);
	closesocket(srv_socket);
	closesocket(clt_socket);

	WSACleanup();

	getchar();
	return 0;
}

