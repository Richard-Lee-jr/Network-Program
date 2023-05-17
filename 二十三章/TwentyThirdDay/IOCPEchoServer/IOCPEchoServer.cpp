// IOCPEchoServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


/*
	��ɶ˿ڵĴ���
	�ٴ�����ɶ˿ڶ���
	�ڽ�����ɶ˿ڶ�����׽���֮�����ϵ��ע�⣺������׽��ֱ��뱻�����ص�����

	�����������ͨ��һ������ʵ�֣�
	#include <windows.h>
	HANDLE CreateIoCompletionPort(HANDLE FileHandle, HANDLE ExistingCompleteionPort, ULONG_PTR CompletionKey, DWORD NumberOfConcurrentThreads);
	�ɹ�ʱ����CP��������ʧ��ʱ����NULL
	�������֣���Բ�ͬĿ�ĵ��û��в�ֵͬ
	FileHandle: ����CP����ʱ����INVALID_HANDLE_VALUE
	ExistingCompleteionPort������CP����ʱ����NULL
	CompletionKey������CP����ʱ����0
	NumberOfConcurrentThreads�������CP��������ڴ���IO���߳��������磬�ò���Ϊ2ʱ��˵�������CP����Ŀ���ͬʱ���е��߳�ʱ���Ϊ2������0����
	ϵͳ��CPU�ĸ������ǿ�ͬʱ���е�����߳���
	ע�⣺ֻ���Դ���CP����ΪĿ�ģ�����ڢٲ������øú���ʱ�����һ�������ž�����������

	������ɶ˿ں��׽���
	FileHandle��Ҫ���ӵ�CP������׽��־����ע�⣺������׽��ֱ��뱻�����ص�����
	ExistingCompleteionPort��Ҫ�����׽��ֵ�CP������
	CompletionKey�����������IO�������Ϣ���ں���GetQueuedCompletionStatus����ʱ����ϸ����
	NumberOfConcurrentThreads: ���ݺ�ֵ��ֻҪ�ú����ڶ���������NULL�ͻ���Ը�ֵ

	ʾ����
	//������ɶ˿ڶ��󣬲�����CP�������2���̴߳���IO
	HANDLE hCPObj = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 2);
	//�����ص��������������Ե��׽���
	SOCKET hSock = WSAScoket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	//������ɶ˿ں��׽���, ��ʱ���Ե���������
	CreateIoCompletionPort((HANDLE)hSock, hCPObj, (DWORD)ioInfo, 0);

	ȷ����ɶ˿�����ɵ�IO���̵߳�IO����
	BOOL GetQueuedCompletionStatus(HANDLE CompletionPort, LPDWORD lpNumberOfBytes, PULONG_PTR lpCompletionKey
		, LPOVERLAPPED* lpOverlapped, DWORD dwMilliseconds);

	CompletionPort: ע���������IO��Ϣ��CP������
	lpNumberOfBytes�����ڱ���IO�����д�������ݴ�С�ı�����ֵַ
	lpCompletionKey�����ڱ���CreateIoCompletionPort�����ĵ���������ֵ�ı�����ֵַ����ͨ��CreateIoCompletionPort����ʱ����ĵ�����������IO�����Ϣ��
	����IO��ɺ󣬲���ϵͳ�ᴫ�ݵ��ò������Ա��������
	lpOverlapped: ���ڱ���WSASend��WSARecv����ʱ���ݵ�OVERLAPPED�ṹ���ַ�ı�����ֵַ��ע������һ��˫ָ��
	dwMilliseconds����ʱ��Ϣ������ָ��ʱ��󽫷���FALSE����������������INFINITEʱ������������ֱ�������IO��Ϣд��CP����

	//������IO����
	#include <winsock2.h>
	int WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSend, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped
		, LPWSAOVERLAPPED_ROUTINE lpCompletionRoutine);

	s: �׽��־�����贫�ݾ����ص�IO���Ե��׽��־�������ص�IO��������ݷ��ص�IO��������
	lpBuffers: WSABUF�ṹ���������ĵ�ַ��WSABUF�д��д���������
	dwBufferCount: �ڶ�������������ĳ���
	lpNumberOfBytesSend: ���ڱ���ʵ�ʷ��ͽ�����ı�����ֵַ
	dwFlags: ���ڸ������ݴ������ԣ��紫��MSG_OOBʱ����OOBģʽ������
	lpOverlapped: WSAOVERLAPPED�ṹ������ĵ�ֵַ��ʹ���¼���������ȷ��������ݴ���
	lpCompletionRoutine������CompletionRoutine��������ڵ�ַ������ͨ���ú���ȷ���Ƿ�������ݴ���

	int WSARecv(SOCKET s, LPWSABUF lpBuffers, DWROD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWROD lpFlags, LPWSAOVERLAPPED lpOverlapped
		, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

	s: �����ص�IO���Ե��׽���
	lpBuffers: ���ڱ���������ݵ�WSABUF�ṹ������ֵ
	dwBufferCount�� �ڶ�����������ĸ���
	lpNumberOfBytesRecvd��������յ������ݴ�С�����ĵ�ֵַ
	lpFlags���������û��ȡ����������Ϣ
	lpOverlapped��WSAOVERLAPPED�ṹ������ĵ�ֵַ
	lpCompletionRoutine������CompletionRoutine��������ڵ�ַ������ͨ���ú���ȷ���Ƿ�������ݽ���

	ע�⣺Ϊ�˽����ص�IO��WSASend��WSARecv�����е��׽��ֲ�����������ص�IO���ԣ�����lpOverlapped����Ӧ������Ч�Ľṹ�������ֵַ������NULL
	�������lpOverlapped����NULL�����һ�������׽��ֽ�������ģʽ������

	���⣬����WSASend��WSARecvͬʱ����Ŀ�괫�䡢��������ʱ����Ҫ�ֱ�������в�����WSAOVERLAPPED�ṹ�������������Ϊ�������ص�IO�Ĺ����У�
	����ϵͳ��ʹ��WSAOVERLAPPED�ṹ�������

	���ͬһĿ�꣬WSASend��WSARecv�Ƿ���Թ���WSAOVERLAPPED�ṹ�������
*/

#include <WinSock2.h>
#include <Windows.h>	//��ȷ��Windows.h �İ�����Winsock2.h��
#include <process.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

//�Զ���ͻ����׽�����Ϣ�ṹ�壬��ᴫ�ݵ�CreateIoCompletionPort�ĵ���������
typedef struct //socket info
{
	SOCKET cltSock;
	SOCKADDR_IN cltAddr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct //buffer info
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode;		//READ or WRITE
}PER_IO_INFO, *LPPER_IO_INFO;

unsigned int WINAPI EchoThreadMain(LPVOID CompletionPortIO);

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

	//������ɶ˿ڶ���
	HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	//����ϵͳ��CPU�ĸ���������ͬʱ���е�����߳�������ɶ˿ڴ���IO
	for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
	}

	//���������׽���
	SOCKET srvSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = PF_INET;
	srvAddr.sin_addr.s_addr = htonl(ADDR_ANY);
	srvAddr.sin_port = htons(_ttoi(argv[1]));
	bind(srvSock, (sockaddr*)&srvAddr, sizeof(srvAddr));
	listen(srvSock, 5);

	int nRecvLen = 0, flags = 0;

	while (true)
	{
		SOCKET cltSock;
		SOCKADDR_IN cltAddr;
		int nCltAddrSize = sizeof(cltAddr);
		memset(&cltAddr, 0, nCltAddrSize);

		puts("wait client connect...");
		cltSock = accept(srvSock, (sockaddr*)&cltAddr, &nCltAddrSize);

		if (INVALID_SOCKET == cltSock)
		{
			puts("accept error!");
			continue;
		}

		//���¿ͻ��˵�ַ��Ϣ
		LPPER_HANDLE_DATA pCltInfo = new PER_HANDLE_DATA;
		pCltInfo->cltSock = cltSock;
		memcpy(&(pCltInfo->cltAddr), &cltAddr, nCltAddrSize);

		//���ͻ����׽������ӵ���ɶ˿�
		CreateIoCompletionPort((HANDLE)cltSock, hComPort, (ULONG_PTR)pCltInfo, 0);

		LPPER_IO_INFO pCltIOInfo = new PER_IO_INFO;
		memset(&(pCltIOInfo->overlapped), 0, sizeof(WSAOVERLAPPED));
		pCltIOInfo->wsaBuf.len = BUF_SIZE;
		pCltIOInfo->wsaBuf.buf = pCltIOInfo->buffer;
		pCltIOInfo->rwMode = READ;

		//����IO�����ȴ�IO���, ������IO��nRecvLen�����壬�����ֱ�ӷ��ص���һѭ����accept����
		WSARecv(cltSock, &(pCltIOInfo->wsaBuf), 1, (LPDWORD)&nRecvLen, (LPDWORD)&flags, &(pCltIOInfo->overlapped), NULL);
	}


	closesocket(srvSock);
	WSACleanup();

	puts("���������...");
	getchar();

	return 0;
}

unsigned int WINAPI EchoThreadMain(LPVOID CompletionPortIO)
{
	//�õ�ͨ��������������ɶ˿ڶ���
	HANDLE hComPort = (HANDLE)CompletionPortIO;
	DWORD dwBytesTrans;

	LPPER_HANDLE_DATA pCltInfo;
	LPPER_IO_INFO pCltIOInfo;
	
	int flags = 0;
	while (true)
	{
		//�ȴ�IO���
		GetQueuedCompletionStatus(hComPort, &dwBytesTrans, (PULONG_PTR)&pCltInfo, (LPOVERLAPPED*)&pCltIOInfo, INFINITE);

		SOCKET cltSock = pCltInfo->cltSock;

		if (pCltIOInfo->rwMode == READ)
		{
			//puts("message received!");
			//printf("dwBytesTrans: %d, len: %d \n", dwBytesTrans, pCltIOInfo->wsaBuf.len);
			if (dwBytesTrans == 0)
			{
				closesocket(cltSock);

				if (pCltIOInfo != nullptr)
				{
					delete pCltIOInfo;
					pCltIOInfo = nullptr;
				}
				
				if (pCltInfo != nullptr)
				{
					delete pCltInfo;
					pCltInfo = nullptr;
				}

				continue;
				
			}//end if (dwBytesTrans == 0)

			memset(&(pCltIOInfo->overlapped), 0, sizeof(OVERLAPPED));
			pCltIOInfo->wsaBuf.len = dwBytesTrans;
			pCltIOInfo->rwMode = WRITE;
			WSASend(cltSock, &(pCltIOInfo->wsaBuf), 1, NULL, 0, &(pCltIOInfo->overlapped), NULL);

			//����ȴ���һ�ν��գ�����һ�ν��ղ�ȷ������ͬһ��������WSASend�ģ��׽��ֻ��������ͻ��˵��׽��֣����������Ҫ��������OVERLAPPED
			pCltIOInfo = new PER_IO_INFO;
			memset(&(pCltIOInfo->overlapped), 0, sizeof(OVERLAPPED));
			memset(&(pCltIOInfo->buffer), 0, BUF_SIZE);
			pCltIOInfo->wsaBuf.len = BUF_SIZE;
			pCltIOInfo->wsaBuf.buf = pCltIOInfo->buffer;

			pCltIOInfo->rwMode = READ;
			WSARecv(pCltInfo->cltSock, &(pCltIOInfo->wsaBuf), 1, NULL, (LPDWORD)&flags, &(pCltIOInfo->overlapped), NULL);
		}//end if (pCltIOInfo->rwMode == READ)
	}

	return 0;
}