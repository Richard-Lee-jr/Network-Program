// UDP_Client.cpp : �������̨Ӧ�ó������ڵ㡣
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

	//��UDP�׽���ת���������׽���
	connect(cltSock, (sockaddr*)&srvAddr, sizeof(srvAddr));


	char Msg[BUF_SIZE] = "0123456789";
	int recvLen = 0;

	//�ͻ��˷����������ݣ��������˵���һ��recvfrom��ͼ�����������ݡ����в�ͨ��
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

		//sendtoʱ������δ�����ַ��Ϣ�������״ε���sendto����ʱ����Ӧ�׽����Զ�����IP�Ͷ˿�
		//ͨ��sendto�����������ݵĹ��̴��¿ɷ�Ϊ����3���׶�
		//����UDP�׽���ע��Ŀ��IP�Ͷ˿ں�
		//�ڴ�������
		//��ɾ��UDP�׽�����ע���Ŀ���ַ��Ϣ

		//ÿ�ε���sendto����ʱ�ظ��������̡�ÿ�α��Ŀ���ַ����˿����ظ�����ͬһUDP�׽�����ͬĿ�괫�����ݡ�
		//��������ע��Ŀ���ַ��Ϣ����sendtoʱ��ע����׽��ֳ�Ϊδ�����׽��֡���֮����ע����Ŀ���ַ���׽��ֳ�Ϊ�����׽��֡���ȻUDP�׽���Ĭ������δ�����׽��֡�
		//��UDP�׽�����ֻ����һ��Ŀ���ַ��������ʱ���Եò�̫����
		//����IPΪ169.21.32.110������9190�Ŷ˿ڹ�׼����3�����ݣ������Ҫ��������sendto�������д��䡣
		//��ʱ��Ҫ�ظ��������׶Σ����������׶��У��ڢ١��۸��׶�ռ����ͨ�Ź��̽�1/3��ʱ�䣬�����ⲿ��ʱ�佫����������Ч�ʡ�
		//��ˣ�Ҫ��ͬһ�������г�ʱ��ͨ�ţ���UDP�׽��ֱ�������׽��ֻ��ṩЧ�ʡ�
		//��ν�UDP�׽��ֱ�������׽��֣�ʹ��connect����ʵ�֡�
		sendto(cltSock, Msg, strlen(Msg), 0, (sockaddr*)&srvAddr, sizeof(srvAddr));

		recvLen = recvfrom(cltSock, Msg, BUF_SIZE - 1, 0, (sockaddr*)&srvAddr, &nSrvAddrLen);
		Msg[recvLen] = 0;

		//�����ӵ�UDP�׽��ֿ���ֱ��ʹ��send �� recv����
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

