// ThreadSynchronization.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>
/*
	用户模式下的同步: 关键代码段
	#include <windows.h>
	void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

	//lpCriticalSection Init...函数中传入需要初始化的CRITICAL_SECTION对象的地址值；反之Del...函数中传入需要解除的CRITICAL_SECTION对象的地址值

	void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

	//lpCriticalSection: 获取(拥有)和释放的CRITICAL_SECTION对象的地址

	==================================================================================================================================

	内核模式下的同步：互斥量、信号量、事件
	基于互斥量的线程同步
	#include <windows.h>
	HANDLE CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName);
	成功时返回创建的互斥量对象句柄，失败返回NULL
	lpMutexAttributes：传递安全相关的配置信息，使用默认安全设置时可以传递NULL
	bInitialOwner：如果为TRUE，则创建出的互斥量对象属于调用该函数的线程，同时进入non-signaled状态；
	如果为FALSE，则创建出的互斥量对象不属于任何线程，此时状态为signaled

	lpName: 用于命名互斥量对象。传入NULL时创建无名的互斥量对象。

	BOOL CloseHandle(HANDLE hObject);
	成功时返回TRUE，失败时返回FALSE
	hObject 要销毁的内核对象的句柄

	通过WaitForSingleObject函数获取互斥量对象

	BOOL ReleaseMutex(HANDLE hMutex);
	成功时返回TRUE，失败时返回FALSE
	hMutex: 需要释放（解除拥有）的互斥量对象句柄

	==================================================================================================================================

	基于信号量的线程同步
	#include <windows.h>
	HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCTSTR lpName);
	成功时返回创建的信号量对象的句柄，失败时返回NULL
	lpSemaphoreAttributes：安全配置信息，采用默认安全设置时传递NULL
	lInitialCount：指定信号量的初始值，应大于0小于lMaximumCount
	lMaximumCount：信号量的最大值。该值为1时，信号量变为只能表示0和1的二进制信号量
	lpName：用于命名信号量对象。传递NULL时创建无名的信号量对象

	使用CloseHandle函数是否信号量对象资源

	使用WaitForSingleObject函数获取信号量，信号量大于0时才会返回，返回的同时将信号量值减一，若减一后值为0，则进入non-signaled状态
	注意：信号量对象的值大于0时为signaled状态，表示可被获取。为0时成为non-signaled状态，表示已被获取完无法再被获取。

	BOOL ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
	成功时返回TRUE，失败时返回FALSE
	hSemaphore：传递需要释放的信号量对象
	lReleaseCount：释放意味着信号量值的增加，通过该参数可以指定增加的值。超过最大值则不增加，返回FALSE
	lpPreviousCount：用于保存修改之前值的变量地址，不需要时可传递NULL

	==================================================================================================================================

	基于事件实现线程同步
	HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttirbutes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName);
	成功时返回创建的事件句柄，失败时返回NULL
	lpEventAttirbutes：安全配置相关参数，采用默认安全配置时传入NULL
	bManualReset：传入TRUE时创建manual-reset模式的事件对象，传入FALSE时创建auto-reset模式的事件对象
	bInitialState：传入TRUE时创建初始化状态为signaled状态的事件对象，传入FALSE时创建non-signaled状态的事件对象
	lpName：用于命名事件对象。传递NULL时创建无名对的事件对象

	使用CloseHandle释放事件对象资源

	使用WaitForSingleObject函数捕获状态为signaled状态的事件对象

	//手动触发signaled 和 non-signaled状态
	BOOL ResetEvent(HANDLE hEvent);	//to the non-signaled
	BOOL ResetEvent(HANDLE hEvent);	//to the signaled
	成功时返回TRUE，失败时返回FALSE
*/


//关键代码段应用
#define NUM_THREAD 50
long long g_llNum = 0;
CRITICAL_SECTION cs;
unsigned WINAPI ThreadInc(void* arg);
unsigned WINAPI ThreadDes(void* arg);

//互斥量实现线程同步
HANDLE hMutex;

//信号量实现线程同步
unsigned WINAPI Read(void* arg);
unsigned WINAPI Accu(void* arg);

static int num;
static HANDLE semOne;
static HANDLE semTwo;

//事件实现线程同步
#define STR_LEN 100
unsigned WINAPI NumberOfA(void* arg);
unsigned WINAPI NumberOfOthers(void* arg);

static char str[STR_LEN];
static HANDLE hEvent;



int _tmain(int argc, _TCHAR* argv[])
{
	/*	//关键代码段和互斥量实现线程同步
	printf("sizeof long long: %d \n", sizeof(long long));

	HANDLE tHandles[NUM_THREAD];

	//关键代码段实现线程同步
	//InitializeCriticalSection(&cs);

	//互斥量实现线程同步
	hMutex = CreateMutex(NULL, FALSE, NULL);

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

	//关键代码段实现线程同步
	//DeleteCriticalSection(&cs);

	//互斥量实现线程同步
	CloseHandle(hMutex);

	printf("result: %lld \n", g_llNum);
	*/

	/*
	HANDLE hThread1, hThread2;
	semOne = CreateSemaphore(NULL, 0, 1, NULL);
	semTwo = CreateSemaphore(NULL, 1, 1, NULL);

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, Read, NULL, 0, NULL);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, Accu, NULL, 0, NULL);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	CloseHandle(semOne);
	CloseHandle(semTwo);
	*/

	HANDLE hThread1, hThread2;
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);
	
	fputs("Input string: ", stdout);
	fgets(str, STR_LEN, stdin);
	SetEvent(hEvent);

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	ResetEvent(hEvent);
	CloseHandle(hEvent);

	puts("end of main");

	getchar();
	return 0;
}


unsigned WINAPI ThreadInc(void* arg)
{
	//关键代码段实现线程同步
	//EnterCriticalSection(&cs);

	//互斥量实现线程同步
	WaitForSingleObject(hMutex, INFINITE);

	for (int i = 0; i < 10000000; i++)
	{
		g_llNum += 1;
	}

	//关键代码段实现线程同步
	//LeaveCriticalSection(&cs);

	//互斥量实现线程同步
	ReleaseMutex(hMutex);

	puts("ThreadInc end.\n");
	return 0;
}

unsigned WINAPI ThreadDes(void* arg)
{
	//关键代码段实现线程同步
	//EnterCriticalSection(&cs);

	//互斥量实现线程同步
	WaitForSingleObject(hMutex, INFINITE);

	for (int i = 0; i < 10000000; i++)
	{
		g_llNum -= 1;
	}

	//关键代码段实现线程同步
	//LeaveCriticalSection(&cs);

	//互斥量实现线程同步
	ReleaseMutex(hMutex);

	puts("ThreadDes end.\n");
	return 0;
}

unsigned WINAPI Read(void* arg)
{
	for (int i = 0; i < 5; i++)
	{
		fputs("Input Num: ", stdout);
		WaitForSingleObject(semTwo, INFINITE);
		scanf("%d", &num);
		ReleaseSemaphore(semOne, 1, NULL);
	}

	puts("Read thread end.");
	return 0;
}

unsigned WINAPI Accu(void* arg)
{
	int sum = 0;
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(semOne, INFINITE);
		sum += num;
		ReleaseSemaphore(semTwo, 1, NULL);
	}

	printf("result: %d \n", sum);
	puts("Accu thread end.");
	return 0;
}

unsigned WINAPI NumberOfA(void* arg)
{
	int nCount = 0;
	WaitForSingleObject(hEvent, INFINITE);
	for (int i = 0; str[i] != 0; i++)
	{
		if (str[i] == 'A')
		{
			nCount++;
		}
	}

	printf("Num of A: %d \n", nCount);
	puts("NumberOfA thread end.");
	return 0;
}

unsigned WINAPI NumberOfOthers(void* arg)
{
	int nCount = 0;
	WaitForSingleObject(hEvent, INFINITE);
	for (int i = 0; str[i] != 0; i++)
	{
		if (str[i] != 'A')
		{
			nCount++;
		}
	}

	printf("Num of others: %d \n", nCount-1);
	puts("NumberOfOthers thread end.");
	return 0;
}