// IOCPEchoServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


/*
	完成端口的创建
	①创建完成端口对象
	②建立完成端口对象和套接字之间的联系。注意：这里的套接字必须被赋予重叠属性

	上述两步骤均通过一个函数实现：
	#include <windows.h>
	HANDLE CreateIoCompletionPort(HANDLE FileHandle, HANDLE ExistingCompleteionPort, ULONG_PTR CompletionKey, DWORD NumberOfConcurrentThreads);
	成功时返回CP对象句柄，失败时返回NULL
	参数部分，针对不同目的调用会有不同值
	FileHandle: 创建CP对象时传递INVALID_HANDLE_VALUE
	ExistingCompleteionPort：创建CP对象时传递NULL
	CompletionKey：创建CP对象时传递0
	NumberOfConcurrentThreads：分配给CP对象的用于处理IO的线程数。例如，该参数为2时，说明分配给CP对象的可以同时运行的线程时最多为2。若传0，则
	系统中CPU的个数就是可同时运行的最大线程数
	注意：只有以创建CP对象为目的（上面第①步）调用该函数时，最后一个参数才具有真正意义

	连接完成端口和套接字
	FileHandle：要连接到CP对象的套接字句柄。注意：这里的套接字必须被赋予重叠属性
	ExistingCompleteionPort：要连接套接字的CP对象句柄
	CompletionKey：传递已完成IO的相关信息，在后面GetQueuedCompletionStatus函数时再详细介绍
	NumberOfConcurrentThreads: 误差传递何值，只要该函数第二个参数非NULL就会忽略该值

	示例：
	//创建完成端口对象，并给该CP对象分配2个线程处理IO
	HANDLE hCPObj = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 2);
	//创建重叠（非阻塞）属性的套接字
	SOCKET hSock = WSAScoket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	//连接完成端口和套接字, 暂时忽略第三个参数
	CreateIoCompletionPort((HANDLE)hSock, hCPObj, (DWORD)ioInfo, 0);

	确认完成端口已完成的IO和线程的IO处理
	BOOL GetQueuedCompletionStatus(HANDLE CompletionPort, LPDWORD lpNumberOfBytes, PULONG_PTR lpCompletionKey
		, LPOVERLAPPED* lpOverlapped, DWORD dwMilliseconds);

	CompletionPort: 注册有已完成IO信息的CP对象句柄
	lpNumberOfBytes：用于保存IO过程中传输的数据大小的变量地址值
	lpCompletionKey：用于保存CreateIoCompletionPort函数的第三个参数值的变量地址值。即通过CreateIoCompletionPort连接时传入的第三个参数（IO相关信息）
	，待IO完成后，操作系统会传递到该参数，以便后续处理
	lpOverlapped: 用于保存WSASend、WSARecv函数时传递的OVERLAPPED结构体地址的变量地址值。注意这是一个双指针
	dwMilliseconds：超时信息。超过指定时间后将返回FALSE并跳出函数。传递INFINITE时，程序将阻塞，直到已完成IO信息写入CP对象

	//非阻塞IO函数
	#include <winsock2.h>
	int WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSend, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped
		, LPWSAOVERLAPPED_ROUTINE lpCompletionRoutine);

	s: 套接字句柄，需传递具有重叠IO属性的套接字句柄，以重叠IO输出。传递非重叠IO会怎样？
	lpBuffers: WSABUF结构体变量数组的地址，WSABUF中存有待传输数据
	dwBufferCount: 第二个参数中数组的长度
	lpNumberOfBytesSend: 用于保存实际发送诫子书的变量地址值
	dwFlags: 用于更改数据传输特性，如传递MSG_OOB时发送OOB模式的数据
	lpOverlapped: WSAOVERLAPPED结构体变量的地址值，使用事件对象，用于确认完成数据传输
	lpCompletionRoutine：传输CompletionRoutine函数的入口地址，可以通过该函数确认是否完成数据传输

	int WSARecv(SOCKET s, LPWSABUF lpBuffers, DWROD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWROD lpFlags, LPWSAOVERLAPPED lpOverlapped
		, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

	s: 赋予重叠IO属性的套接字
	lpBuffers: 用于保存接收数据的WSABUF结构体数组值
	dwBufferCount： 第二个参数数组的个数
	lpNumberOfBytesRecvd：保存接收到的数据大小变量的地址值
	lpFlags：用于设置或读取传输特性信息
	lpOverlapped：WSAOVERLAPPED结构体变量的地址值
	lpCompletionRoutine：传输CompletionRoutine函数的入口地址，可以通过该函数确认是否完成数据接收

	注意：为了进行重叠IO，WSASend和WSARecv函数中的套接字参数必须具有重叠IO属性，而且lpOverlapped参数应传递有效的结构体变量地址值而不是NULL
	。如果向lpOverlapped传递NULL，则第一个参数套接字将以阻塞模式工作。

	此外，利用WSASend或WSARecv同时向多个目标传输、接收数据时，需要分别传入第六感参数的WSAOVERLAPPED结构体变量。这是因为，进行重叠IO的过程中，
	操作系统将使用WSAOVERLAPPED结构体变量。

	针对同一目标，WSASend和WSARecv是否可以共用WSAOVERLAPPED结构体变量？
*/

#include <WinSock2.h>
#include <Windows.h>	//需确保Windows.h 的包含在Winsock2.h后
#include <process.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

//自定义客户端套接字信息结构体，后会传递到CreateIoCompletionPort的第三个参数
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

	//创建完成端口对象
	HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	//分配系统中CPU的个数，即可同时运行的最大线程数给完成端口处理IO
	for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
	}

	//创建监听套接字
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

		//更新客户端地址信息
		LPPER_HANDLE_DATA pCltInfo = new PER_HANDLE_DATA;
		pCltInfo->cltSock = cltSock;
		memcpy(&(pCltInfo->cltAddr), &cltAddr, nCltAddrSize);

		//将客户端套接字连接到完成端口
		CreateIoCompletionPort((HANDLE)cltSock, hComPort, (ULONG_PTR)pCltInfo, 0);

		LPPER_IO_INFO pCltIOInfo = new PER_IO_INFO;
		memset(&(pCltIOInfo->overlapped), 0, sizeof(WSAOVERLAPPED));
		pCltIOInfo->wsaBuf.len = BUF_SIZE;
		pCltIOInfo->wsaBuf.buf = pCltIOInfo->buffer;
		pCltIOInfo->rwMode = READ;

		//调用IO函数等待IO完成, 非阻塞IO，nRecvLen无意义，这里会直接返回到下一循环的accept阻塞
		WSARecv(cltSock, &(pCltIOInfo->wsaBuf), 1, (LPDWORD)&nRecvLen, (LPDWORD)&flags, &(pCltIOInfo->overlapped), NULL);
	}


	closesocket(srvSock);
	WSACleanup();

	puts("任意键继续...");
	getchar();

	return 0;
}

unsigned int WINAPI EchoThreadMain(LPVOID CompletionPortIO)
{
	//得到通过参数传来的完成端口对象
	HANDLE hComPort = (HANDLE)CompletionPortIO;
	DWORD dwBytesTrans;

	LPPER_HANDLE_DATA pCltInfo;
	LPPER_IO_INFO pCltIOInfo;
	
	int flags = 0;
	while (true)
	{
		//等待IO完成
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

			//这里等待下一次接收，而下一次接收不确定来自同一个（上面WSASend的）套接字还是其他客户端的套接字，因此这里需要重新申请OVERLAPPED
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