// AsynNotiServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100

/*
	异步通知IO模型
	相关函数
	int WSAEventSelect(SOCKET s, WSAEVENT hEventObject, long lNetworkEvent);
	函数说明：该函数用于指定某一套接字为事件监视对象
	返回值：成功时返回0，失败时返回SOCKET_ERROR
	s：监视对象的套接字句柄
	hEventObject：传递事件对象句柄以验证事件发生与否
	lNetworkEvents：希望监视的事件的类型信息

	补充说明：只要传入参数s的套接字内只要发生lNetworkEvent中指定的事件之一，WSAEventSelect函数就将hEventObject句柄所指向内核对象改为signaled
	状态。因此，该函数又称“连接事件对象和套接字的函数”
	另外，无论事件发生与否，WSAEventSelect函数调用后都会直接返回，所以调用后不会阻塞，可以执行其他任务。
	即该函数是以异步方式工作的。

	以下是lNetworkEvent参数可选的事件类型信息，可以通过位或运算同时指定多个信息：
	FD_READ: 是否存在需要接收的数据
	FD_WRITE: 能否以非阻塞方式传输数据
	FD_OOB: 是否收到带外数据
	FD_ACCEPT: 是否有新的连接请求
	FD_CLOSE: 是否有断开连接的请求

	从WSAEventSelect函数的说明中可以看出，还需要补充如下内容：
	①WSAEventSelect函数的第二个参数用到的事件对象的创建方法
	②调用WSAEventSelect函数后发生事件时的验证方法
	③验证事件发生后事件类型的查看方法

	manual-reset模式事件对象的其他创建方法
	创建内核资源
	WSAEVENT WSACreateEvent(void);
	成功时返回事件对象句柄，失败时返回WSA_INVALID_EVENT
	#define WSAEVENT HANDLE

	释放句柄资源
	BOOL WSACloseEvent(WSAEVENT hEvent);
	成功时返回TRUE，失败时返回FALSE

	等待内核资源
	DWORD WSAWaitForMultipleEvents(DWROD cEvents, const WSAEvent* lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable);
	成功时返回发送事件的对象信息，失败时返回WSA_INVALID_EVENT
	cEvents: 需要验证是否转为signaled状态的事件对象的个数
	lphEvents：存有事件对象句柄的数组地址值
	fWaitAll：传递TRUE时，所有对象均为signaled状态时返回。传递FALSE时，只要其中一个为signaled状态就返回
	dwTimeout：以ms为单位指定超时，传递WSA_INFINITE时，直到变为signaled状态时才返回
	fAlertable：传递TRUE时进入alertable（可警告等待）状态，暂未使用
	返回值：返回值减去常量WSA_WAITEVENT_0时，可以得到转变为signaled状态的事件对象句柄对应的索引，通过该索引在第二个参数指定的句柄数组中找到
	触发事件的套接字句柄。如果有多个事件对象变为signaled状态，则会得到其中较小的值。发生超时则返回WSA_WAIT_TIMEOUT

	区分事件类型
	int WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject, LPWSANETWORKEVENTS lpNetworkEvents);
	成功时返回0，失败时返回SOCKET_ERROR
	s: 发生事件的对应的套接字句柄
	hEventObject：与套接字相连（由WSAEventSelect函数调用绑定）的signaled状态的事件对象句柄
	lpNetworkEvents：保存发生的事件类型信息和错误信息的WSANETWORKEVENTS结构体变量地址值
	注意：该函数将manual-reset模式的事件对象改为non-signaled状态，所以得到发生的事件后，不必再单独调用ResetEvent函数。

	WSANETWORKEVENTS结构体
	typedef struct _WSANETWORKEVENTS
	{
		long lNetworkEvents;
		int iErrorCode[FD_MAX_EVENTS];
	}WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

	上述结构体的lNetworkEvents成员将保存发生的事件信息。与WSAEventSelect函数的第三个参数相同，需要接收数据时，该成员为FD_READ;有连接请求时，为
	FD_ACCEPT。因此可通过以下方式查看发生的事件类型：
	WSANETWORKEVENTS netEvents;
	WSAEnumNetworkEvents(hSock, hEvent, &netEvents);
	if (netEvents.lNetworkEvents & FD_ACCEPT)
	{
		//FD_ACCEPT事件的处理
	}
	if (netEvents.lNetworkEvents & FD_READ)
	{
		//FD_READ事件的处理
	}

	另外，错误信息将保存到iErrorCode数组成员中。
	如果发生FD_READ相关错误，则在iErrorCode[FD_READ_BIT]中保存除0以外的其他值
	若发生FD_WRITE相关错误，则在iErrorCode[FD_WRITE_BIT]中保存除0以外的其他值
	如下示例：
	WSANETWORKEVENTS netEvents;
	WSAEnumNetworkEvents(hSock, hEvent, &netEvents);
	if (netEvents.iErrorCode[FD_READ_BIT] != 0)
	{
		//发生FD_READ事件相关错误
	}

	if (netEvents.iErrorCode[FD_WRITE_BIT] != 0)
	{
		//发生FD_WRITE事件相关错误
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

	
	//创建manual-reset模式的事件内核对象
	WSAEVENT newEvents = WSACreateEvent();

	if (SOCKET_ERROR == WSAEventSelect(srvSock, newEvents, FD_ACCEPT))
	{
		puts("WSAEventSelect error!");
		closesocket(srvSock);
		WSACleanup();
		return -1;
	}

	//创建异步通知IO模型
	SOCKET hScokArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
	int numOfCltSock = 0;

	//注意这里hScokArr 和 hEventArr通过numOfCltSock一一对应
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

		//遍历得到所有触发的事件及对应套接字句柄
		for (int i = startIdx; i < numOfCltSock; i++)
		{
			//这里逐个判断事件释放触发
			int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);

			printf("sigEventIdx: %d, i: %d \n", sigEventIdx, i);

			//判断hEventArr[i]事件是否触发
			if (sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT)
			{
				puts("wait failed or timeout!");
				continue;
			}
			else
			{
				//触发
				sigEventIdx = i;
				
				//得到触发类型
				WSAEnumNetworkEvents(hScokArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);

				//判断是否是监听套接字的接受连接事件
				if (netEvents.lNetworkEvents & FD_ACCEPT)
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						printf("WSAEnumNetworkEvents--socket《%d》 accept error! errorcode: %d \n"
							, hScokArr[sigEventIdx], netEvents.iErrorCode[FD_ACCEPT_BIT]);
						break;
					}

					nCltAddrSize = sizeof(cltAddr);
					SOCKET cltSock = accept(hScokArr[sigEventIdx], (sockaddr*)&cltAddr, &nCltAddrSize);
					if (cltSock == INVALID_SOCKET)
					{
						printf("WSAEnumNetworkEvents--socket《%d》 accept error!\n", hScokArr[sigEventIdx]);
						break;
					}

					//添加到异步通知IO模型中
					newEvents = WSACreateEvent();
					WSAEventSelect(cltSock, newEvents, FD_READ | FD_CLOSE);
					//这里也可判断绑定套接字和事件是否成功

					hScokArr[numOfCltSock] = cltSock;
					hEventArr[numOfCltSock] = newEvents;
					numOfCltSock++;
					printf("client《%d》 connected...\n", cltSock);
				}//end if (netEvents.lNetworkEvents & FD_ACCEPT)

				//判断是否是客户端套接字的数据接收事件
				if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						printf("client<%d> read event error, msg: %d\n", hScokArr[sigEventIdx], netEvents.iErrorCode[FD_READ_BIT]);
						break;
					}

					nRecvLen = recv(hScokArr[sigEventIdx], msg, BUF_SIZE, 0);
					msg[nRecvLen] = 0;
					//回发
					send(hScokArr[sigEventIdx], msg, nRecvLen, 0);
				}

				//判断是否是客户端套接字的断开连接事件
				if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						printf("client<%d> close event error, msg: %d\n", hScokArr[sigEventIdx], netEvents.iErrorCode[FD_CLOSE_BIT]);
						break;
					}

					//释放套接字对应事件内核对象资源
					WSACloseEvent(hEventArr[sigEventIdx]);
					//关闭套接字
					closesocket(hScokArr[sigEventIdx]);

					//更新数组资源
					numOfCltSock--;
					CompressSockets(hScokArr, sigEventIdx, numOfCltSock);
					CompressEvents(hEventArr, sigEventIdx, numOfCltSock);
				}
			}

		}
		
	}

	//这里无需释放监听套接字资源？

	WSACleanup();
	puts("任意键继续...");
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