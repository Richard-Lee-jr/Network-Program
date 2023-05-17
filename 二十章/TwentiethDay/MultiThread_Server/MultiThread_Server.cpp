// MultiThread_Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
//#include <windows.h>
#include <process.h>
#include <WinSock2.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI HandleClnt(void* arg);
void SendMsg(char* arg, int len);

HANDLE hMutex;
int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];

void WriteRunLog(LPCSTR lpszLog, int len);

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
		printf("WSAStartup error!\n");
		return -1;
	}

	SOCKET srvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srvSock)
	{
		printf("socket error!\n");
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
		printf("bind error!\n");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(srvSock, 5))
	{
		printf("listen error!\n");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int nCltAddrSize = sizeof(cltAddr);

	hMutex = CreateMutex(NULL, FALSE, NULL);

	
	while (true)
	{
		//���������߳�
		nCltAddrSize = sizeof(cltAddr);
		puts("wait for client connect...");
		SOCKET cltSock = accept(srvSock, (sockaddr*)&cltAddr, &nCltAddrSize);
		if (cltSock == INVALID_SOCKET)
		{
			printf("accept error\n");
			continue;
		}

		//�ȴ���������������
		WaitForSingleObject(hMutex, INFINITE);
		clntSocks[clntCnt++] = cltSock;
		ReleaseMutex(hMutex);

		//��������׽��ֵ���Ϣ�����߳�
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&cltSock, 0, NULL);
		printf("Connected Client IP: %s \n", inet_ntoa(cltAddr.sin_addr));

	}

	CloseHandle(hMutex);
	closesocket(srvSock);
	WSACleanup();

	puts("main thread end.");
	puts("���������...");
	getchar();

	return 0;
}

unsigned WINAPI HandleClnt(void* arg)
{
	SOCKET cltSock = *((SOCKET*)arg);

	int nRecvLen = 0;
	char Msg[BUF_SIZE] = {};
	char Log[2*BUF_SIZE] = {};
	while ( (nRecvLen = recv(cltSock, Msg, BUF_SIZE, 0)) != 0 )
	{
		Msg[nRecvLen] = 0;

		//sprintf(Log, "recv msg from client��%d��: %s\n", cltSock, Msg);
		//WriteRunLog(Log, strlen(Log));
		SendMsg(Msg, nRecvLen);
	}

	//���ͻ��˶Ͽ����ӣ������׽��������������Ӧsocket
	WaitForSingleObject(hMutex, INFINITE);
	//�ҵ�Ҫ������׽��֣��Ӹ�λ�ÿ�ʼ������Ԫ��ǰ�Ƹ���ɾ��
	//˫ָ��ʵ�ָ���ɾ��
	int slow = 0;
	int fast = 0;

	for (; fast < clntCnt; fast++)
	{
		if (clntSocks[fast] == cltSock)
		{
			continue;
		}

		clntSocks[slow++] = clntSocks[fast];
	}
	clntSocks[slow] = INVALID_SOCKET;
	
	clntCnt--;
	ReleaseMutex(hMutex);
	closesocket(cltSock);

	//sprintf(Log, "Client %d Disconnected...\n", cltSock);
	//WriteRunLog(Log, strlen(Log));

	return 0;

}

void SendMsg(char* arg, int len)
{
	//�ظ����пͻ���
	WaitForSingleObject(hMutex, INFINITE);
	char Log[2*BUF_SIZE] = {};
	for (int i = 0; i < clntCnt; i++)
	{
		//sprintf(Log, "Send to client ��%d�� msg: %s\n", clntSocks[i], len);
		//WriteRunLog(Log, strlen(Log));
		send(clntSocks[i], arg, len, 0);
	}

	ReleaseMutex(hMutex);
}

/*
void WriteRunLog(LPCSTR lpszLog, int len)
{
	char AppDir[MAX_PATH];
	::GetModuleFileName(::GetModuleHandle(NULL), AppDir, MAX_PATH);

	static CRITICAL_SECTION csWriteLog;
	static BOOL bFirstLog = TRUE;	

	::EnterCriticalSection(&csWriteLog);

	CFile file;
	if (file.Open("./ServerLog.txt", CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
	{
		file.SeekToEnd();
		file.Write(lpszLog, len);
		file.Close();
	}

	::LeaveCriticalSection(&csWriteLog);
}
*/