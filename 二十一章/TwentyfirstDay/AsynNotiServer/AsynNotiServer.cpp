// AsynNotiServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100

/*
	�첽֪ͨIOģ��
	��غ���
	int WSAEventSelect(SOCKET s, WSAEVENT hEventObject, long lNetworkEvent);
	����˵�����ú�������ָ��ĳһ�׽���Ϊ�¼����Ӷ���
	����ֵ���ɹ�ʱ����0��ʧ��ʱ����SOCKET_ERROR
	s�����Ӷ�����׽��־��
	hEventObject�������¼�����������֤�¼��������
	lNetworkEvents��ϣ�����ӵ��¼���������Ϣ

	����˵����ֻҪ�������s���׽�����ֻҪ����lNetworkEvent��ָ�����¼�֮һ��WSAEventSelect�����ͽ�hEventObject�����ָ���ں˶����Ϊsignaled
	״̬����ˣ��ú����ֳơ������¼�������׽��ֵĺ�����
	���⣬�����¼��������WSAEventSelect�������ú󶼻�ֱ�ӷ��أ����Ե��ú󲻻�����������ִ����������
	���ú��������첽��ʽ�����ġ�

	������lNetworkEvent������ѡ���¼�������Ϣ������ͨ��λ������ͬʱָ�������Ϣ��
	FD_READ: �Ƿ������Ҫ���յ�����
	FD_WRITE: �ܷ��Է�������ʽ��������
	FD_OOB: �Ƿ��յ���������
	FD_ACCEPT: �Ƿ����µ���������
	FD_CLOSE: �Ƿ��жϿ����ӵ�����

	��WSAEventSelect������˵���п��Կ���������Ҫ�����������ݣ�
	��WSAEventSelect�����ĵڶ��������õ����¼�����Ĵ�������
	�ڵ���WSAEventSelect���������¼�ʱ����֤����
	����֤�¼��������¼����͵Ĳ鿴����

	manual-resetģʽ�¼������������������
	�����ں���Դ
	WSAEVENT WSACreateEvent(void);
	�ɹ�ʱ�����¼���������ʧ��ʱ����WSA_INVALID_EVENT
	#define WSAEVENT HANDLE

	�ͷž����Դ
	BOOL WSACloseEvent(WSAEVENT hEvent);
	�ɹ�ʱ����TRUE��ʧ��ʱ����FALSE

	�ȴ��ں���Դ
	DWORD WSAWaitForMultipleEvents(DWROD cEvents, const WSAEvent* lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable);
	�ɹ�ʱ���ط����¼��Ķ�����Ϣ��ʧ��ʱ����WSA_INVALID_EVENT
	cEvents: ��Ҫ��֤�Ƿ�תΪsignaled״̬���¼�����ĸ���
	lphEvents�������¼��������������ֵַ
	fWaitAll������TRUEʱ�����ж����Ϊsignaled״̬ʱ���ء�����FALSEʱ��ֻҪ����һ��Ϊsignaled״̬�ͷ���
	dwTimeout����msΪ��λָ����ʱ������WSA_INFINITEʱ��ֱ����Ϊsignaled״̬ʱ�ŷ���
	fAlertable������TRUEʱ����alertable���ɾ���ȴ���״̬����δʹ��
	����ֵ������ֵ��ȥ����WSA_WAITEVENT_0ʱ�����Եõ�ת��Ϊsignaled״̬���¼���������Ӧ��������ͨ���������ڵڶ�������ָ���ľ���������ҵ�
	�����¼����׽��־��������ж���¼������Ϊsignaled״̬�����õ����н�С��ֵ��������ʱ�򷵻�WSA_WAIT_TIMEOUT

	�����¼�����
	int WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject, LPWSANETWORKEVENTS lpNetworkEvents);
	�ɹ�ʱ����0��ʧ��ʱ����SOCKET_ERROR
	s: �����¼��Ķ�Ӧ���׽��־��
	hEventObject�����׽�����������WSAEventSelect�������ð󶨣���signaled״̬���¼�������
	lpNetworkEvents�����淢�����¼�������Ϣ�ʹ�����Ϣ��WSANETWORKEVENTS�ṹ�������ֵַ
	ע�⣺�ú�����manual-resetģʽ���¼������Ϊnon-signaled״̬�����Եõ��������¼��󣬲����ٵ�������ResetEvent������

	WSANETWORKEVENTS�ṹ��
	typedef struct _WSANETWORKEVENTS
	{
		long lNetworkEvents;
		int iErrorCode[FD_MAX_EVENTS];
	}WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

	�����ṹ���lNetworkEvents��Ա�����淢�����¼���Ϣ����WSAEventSelect�����ĵ�����������ͬ����Ҫ��������ʱ���ó�ԱΪFD_READ;����������ʱ��Ϊ
	FD_ACCEPT����˿�ͨ�����·�ʽ�鿴�������¼����ͣ�
	WSANETWORKEVENTS netEvents;
	WSAEnumNetworkEvents(hSock, hEvent, &netEvents);
	if (netEvents.lNetworkEvents & FD_ACCEPT)
	{
		//FD_ACCEPT�¼��Ĵ���
	}
	if (netEvents.lNetworkEvents & FD_READ)
	{
		//FD_READ�¼��Ĵ���
	}

	���⣬������Ϣ�����浽iErrorCode�����Ա�С�
	�������FD_READ��ش�������iErrorCode[FD_READ_BIT]�б����0���������ֵ
	������FD_WRITE��ش�������iErrorCode[FD_WRITE_BIT]�б����0���������ֵ
	����ʾ����
	WSANETWORKEVENTS netEvents;
	WSAEnumNetworkEvents(hSock, hEvent, &netEvents);
	if (netEvents.iErrorCode[FD_READ_BIT] != 0)
	{
		//����FD_READ�¼���ش���
	}

	if (netEvents.iErrorCode[FD_WRITE_BIT] != 0)
	{
		//����FD_WRITE�¼���ش���
	}
*/

void CompressSockets(SOCKET hSockArr[], int idx, int total);
void CompressEvents(WSAEVENT hEventArr[], int idx, int total);


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		puts("argc error!");
		return -1;
	}

	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2, ), &wsaData))
	{
		puts("WSAStartup error!");
		return -1;
	}

	SOCKET srvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == srvSock)
	{
		puts("socket error!");
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

	
	//����manual-resetģʽ���¼��ں˶���
	WSAEVENT newEvents = WSACreateEvent();

	if (SOCKET_ERROR == WSAEventSelect(srvSock, newEvents, FD_ACCEPT))
	{
		puts("WSAEventSelect error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	//�����첽֪ͨIOģ��
	SOCKET hScokArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
	int numOfCltSock = 0;

	//ע������hScokArr �� hEventArrͨ��numOfCltSockһһ��Ӧ
	hScokArr[numOfCltSock] = srvSock;
	hEventArr[numOfCltSock] = newEvents;
	numOfCltSock++;

	WSANETWORKEVENTS netEvents;

	int posInfo = 0, startIdx = 0;

	SOCKADDR_IN cltAddr;
	memset(&cltAddr, 0, sizeof(cltAddr));
	int nCltAddrSize = sizeof(cltAddr);
	
	int nRecvLen = 0;
	char msg[BUF_SIZE] = {};

	while (true)
	{
		posInfo = WSAWaitForMultipleEvents(numOfCltSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
		startIdx = posInfo - WSA_WAIT_EVENT_0;

		//�����õ����д������¼�����Ӧ�׽��־��
		for (int i = startIdx; i < numOfCltSock; i++)
		{
			//��������ж��¼��ͷŴ���
			int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);

			printf("sigEventIdx: %d, i: %d \n", sigEventIdx, i);

			//�ж�hEventArr[i]�¼��Ƿ񴥷�
			if (sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT)
			{
				puts("wait failed or timeout!");
				continue;
			}
			else
			{
				//����
				sigEventIdx = i;
				
				//�õ���������
				WSAEnumNetworkEvents(hScokArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);

				//�ж��Ƿ��Ǽ����׽��ֵĽ��������¼�
				if (netEvents.lNetworkEvents & FD_ACCEPT)
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						printf("WSAEnumNetworkEvents--socket��%d�� accept error! errorcode: %d \n"
							, hScokArr[sigEventIdx], netEvents.iErrorCode[FD_ACCEPT_BIT]);
						break;
					}

					nCltAddrSize = sizeof(cltAddr);
					SOCKET cltSock = accept(hScokArr[sigEventIdx], (sockaddr*)&cltAddr, &nCltAddrSize);
					if (cltSock == INVALID_SOCKET)
					{
						printf("WSAEnumNetworkEvents--socket��%d�� accept error!\n", hScokArr[sigEventIdx]);
						break;
					}

					//��ӵ��첽֪ͨIOģ����
					newEvents = WSACreateEvent();
					WSAEventSelect(cltSock, newEvents, FD_READ | FD_CLOSE);
					//����Ҳ���жϰ��׽��ֺ��¼��Ƿ�ɹ�

					hScokArr[numOfCltSock] = cltSock;
					hEventArr[numOfCltSock] = newEvents;
					numOfCltSock++;
					printf("client��%d�� connected...\n", cltSock);
				}//end if (netEvents.lNetworkEvents & FD_ACCEPT)

				//�ж��Ƿ��ǿͻ����׽��ֵ����ݽ����¼�
				if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						printf("client<%d> read event error, msg: %d\n", hScokArr[sigEventIdx], netEvents.iErrorCode[FD_READ_BIT]);
						break;
					}

					nRecvLen = recv(hScokArr[sigEventIdx], msg, BUF_SIZE, 0);
					msg[nRecvLen] = 0;
					//�ط�
					send(hScokArr[sigEventIdx], msg, nRecvLen, 0);
				}

				//�ж��Ƿ��ǿͻ����׽��ֵĶϿ������¼�
				if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						printf("client<%d> close event error, msg: %d\n", hScokArr[sigEventIdx], netEvents.iErrorCode[FD_CLOSE_BIT]);
						break;
					}

					//�ͷ��׽��ֶ�Ӧ�¼��ں˶�����Դ
					WSACloseEvent(hEventArr[sigEventIdx]);
					//�ر��׽���
					closesocket(hScokArr[sigEventIdx]);

					//����������Դ
					numOfCltSock--;
					CompressSockets(hScokArr, sigEventIdx, numOfCltSock);
					CompressEvents(hEventArr, sigEventIdx, numOfCltSock);
				}
			}

		}
		
	}

	//���������ͷż����׽�����Դ��

	WSACleanup();
	puts("���������...");
	getchar();

	return 0;
}

void CompressSockets(SOCKET hSockArr[], int idx, int total)
{
	for (int i = idx; i < total; i++)
	{
		hSockArr[i] = hSockArr[i + 1];
	}
}

void CompressEvents(WSAEVENT hEventArr[], int idx, int total)
{
	for (int i = idx; i < total; i++)
	{
		hEventArr[i] = hEventArr[i + 1];
	}
}