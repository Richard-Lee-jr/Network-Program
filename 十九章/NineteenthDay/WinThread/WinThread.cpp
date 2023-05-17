// WinThread.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

/*
	线程安全标准C函数,签名如下：
	#include <process.h>

	uintptr_t _beginthreadex(
		void* security,
		unsigned stack_size,
		unsigned (* start_address)(void*),
		void* arglist,
		unsigned initflag,
		unsigned* thrdaddr);
	成功时返回线程句柄，失败时返回0
	
	security: 线程安全相关信息，使用默认设置时传递NULL
	stack_size: 要分配给线程的栈大小，传递0时生成默认大小的栈
	start_address: 传递线程的main函数信息
	arglist: 调用线程main函数的传递的参数信息
	initflag: 用于指定线程创建后的行为，传递0时，线程创建后立即进入可执行状态
	thrdaddr: 用于保存线程ID的变量地址值

	句柄、内核对象以及ID之间的关系
	线程也属于操作系统管理的内核资源，因此会伴随着内核对象的创建，并为了引用内核对象而返回句柄。
	可以通过句柄区分内核对象，通过内核对象可以区分线程。及可通过句柄区分线程。
	那线程ID又是什么呢? 上面的_beginthreadex函数的最后一个输出参数返回线程ID，
	句柄的整数值在不同的进程中可能重复出现，但线程ID在却不会在范围内不会重复出现。
	所以线程ID用于区分操作系统创建的所有线程。


	WaitForSingleObject 和 WaitForMultipleObjects
	#include <windows.h>
	DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

	hHandle: 查看状态的内核对象句柄
	dwMilliseconds: 以ms为单位指定超时，传递INFINTE时函数不会返回（即永久阻塞），直到内核对象被触发
	进入signaled状态返回WAIT_OBJECT_0，超时返回WAIT_TIMEOUT，调用失败时返回WAIT_FAILED。

	DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* plHandles, BOOL bWaitAll, DWORD dwMilliseconds);
	nCount:  需验证的内核对象数
	lpHandles: 存有内核对象句柄的数组地址值
	bWaitAll: 如果为TRUE，则所有内容对象全部变为signaled时返回；如果为FALSE，则只要有1个验证对象的状态变为signaled就会返回
	dwMilliseconds: 以ms为单位指定超时，传递INFINITE时函数不会返回，直到内核对象变为signaled状态
*/


unsigned WINAPI ThreadFunc(void* arg);

#define NUM_THREAD 50
long long g_llNum = 0;
unsigned WINAPI ThreadInc(void* arg);
unsigned WINAPI ThreadDes(void* arg);

int _tmain(int argc, _TCHAR* argv[])
{
	/*
	int lpParam = 5;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&lpParam, 0, NULL);
	if (hThread == 0)
	{
		puts("_beginthreadex error!");
		return -1;
	}
	//Sleep(3000);

	//使用WaitForSingleObject函数等待线程结束
	DWORD dwRet;
	if ( (dwRet = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED )
	{
		puts("thread wait error!");
		return -1;
	}

	printf("thread wait result: %s \n", (dwRet == WAIT_OBJECT_0) ? "线程正常结束" : "超时");
	*/

	printf("sizeof long long: %d \n", sizeof(long long));
	//这里开NUM_THREAD线程, 其中NUM_THREAD/2个线程对全局变量指向+1操作，NUM_THREAD/2个线程对全局变量执行减1操作，想要的最终效果为值不发生变化
	//这里不会达到预期效果，需使用线程同步技术
	HANDLE tHandles[NUM_THREAD];
	for (int i = 0; i < NUM_THREAD; i++)
	{
		if (i % 2)
		{
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadInc, NULL, 0, NULL);
		}
		else
		{
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadDes, NULL, 0, NULL);
		}
	}

	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);
	printf("result: %lld \n", g_llNum);

	puts("end of main");

	getchar();
	return 0;
}

unsigned WINAPI ThreadFunc(void* arg)
{
	int i = 0;
	int nCount = *((int*)arg);
	for (; i < nCount; i++)
	{
		Sleep(1000);
		puts("running thread");
	}

	return 0;
}

unsigned WINAPI ThreadInc(void* arg)
{
	for (int i = 0; i < 10000000; i++)
	{
		g_llNum += 1;
	}

	puts("ThreadInc end.\n");
	return 0;
}

unsigned WINAPI ThreadDes(void* arg)
{
	for (int i = 0; i < 10000000; i++)
	{
		g_llNum -= 1;
	}
	puts("ThreadDes end.\n");
	return 0;
}
//4295875883
//-2108812