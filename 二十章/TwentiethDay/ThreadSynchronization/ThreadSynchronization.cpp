// ThreadSynchronization.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>
/*
	�û�ģʽ�µ�ͬ��: �ؼ������
	#include <windows.h>
	void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

	//lpCriticalSection Init...�����д�����Ҫ��ʼ����CRITICAL_SECTION����ĵ�ֵַ����֮Del...�����д�����Ҫ�����CRITICAL_SECTION����ĵ�ֵַ

	void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
	void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

	//lpCriticalSection: ��ȡ(ӵ��)���ͷŵ�CRITICAL_SECTION����ĵ�ַ

	==================================================================================================================================

	�ں�ģʽ�µ�ͬ�������������ź������¼�
	���ڻ��������߳�ͬ��
	#include <windows.h>
	HANDLE CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName);
	�ɹ�ʱ���ش����Ļ�������������ʧ�ܷ���NULL
	lpMutexAttributes�����ݰ�ȫ��ص�������Ϣ��ʹ��Ĭ�ϰ�ȫ����ʱ���Դ���NULL
	bInitialOwner�����ΪTRUE���򴴽����Ļ������������ڵ��øú������̣߳�ͬʱ����non-signaled״̬��
	���ΪFALSE���򴴽����Ļ��������������κ��̣߳���ʱ״̬Ϊsignaled

	lpName: �����������������󡣴���NULLʱ���������Ļ���������

	BOOL CloseHandle(HANDLE hObject);
	�ɹ�ʱ����TRUE��ʧ��ʱ����FALSE
	hObject Ҫ���ٵ��ں˶���ľ��

	ͨ��WaitForSingleObject������ȡ����������

	BOOL ReleaseMutex(HANDLE hMutex);
	�ɹ�ʱ����TRUE��ʧ��ʱ����FALSE
	hMutex: ��Ҫ�ͷţ����ӵ�У��Ļ�����������

	==================================================================================================================================

	�����ź������߳�ͬ��
	#include <windows.h>
	HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCTSTR lpName);
	�ɹ�ʱ���ش������ź�������ľ����ʧ��ʱ����NULL
	lpSemaphoreAttributes����ȫ������Ϣ������Ĭ�ϰ�ȫ����ʱ����NULL
	lInitialCount��ָ���ź����ĳ�ʼֵ��Ӧ����0С��lMaximumCount
	lMaximumCount���ź��������ֵ����ֵΪ1ʱ���ź�����Ϊֻ�ܱ�ʾ0��1�Ķ������ź���
	lpName�����������ź������󡣴���NULLʱ�����������ź�������

	ʹ��CloseHandle�����Ƿ��ź���������Դ

	ʹ��WaitForSingleObject������ȡ�ź������ź�������0ʱ�Ż᷵�أ����ص�ͬʱ���ź���ֵ��һ������һ��ֵΪ0�������non-signaled״̬
	ע�⣺�ź��������ֵ����0ʱΪsignaled״̬����ʾ�ɱ���ȡ��Ϊ0ʱ��Ϊnon-signaled״̬����ʾ�ѱ���ȡ���޷��ٱ���ȡ��

	BOOL ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
	�ɹ�ʱ����TRUE��ʧ��ʱ����FALSE
	hSemaphore��������Ҫ�ͷŵ��ź�������
	lReleaseCount���ͷ���ζ���ź���ֵ�����ӣ�ͨ���ò�������ָ�����ӵ�ֵ���������ֵ�����ӣ�����FALSE
	lpPreviousCount�����ڱ����޸�֮ǰֵ�ı�����ַ������Ҫʱ�ɴ���NULL

	==================================================================================================================================

	�����¼�ʵ���߳�ͬ��
	HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttirbutes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName);
	�ɹ�ʱ���ش������¼������ʧ��ʱ����NULL
	lpEventAttirbutes����ȫ������ز���������Ĭ�ϰ�ȫ����ʱ����NULL
	bManualReset������TRUEʱ����manual-resetģʽ���¼����󣬴���FALSEʱ����auto-resetģʽ���¼�����
	bInitialState������TRUEʱ������ʼ��״̬Ϊsignaled״̬���¼����󣬴���FALSEʱ����non-signaled״̬���¼�����
	lpName�����������¼����󡣴���NULLʱ���������Ե��¼�����

	ʹ��CloseHandle�ͷ��¼�������Դ

	ʹ��WaitForSingleObject��������״̬Ϊsignaled״̬���¼�����

	//�ֶ�����signaled �� non-signaled״̬
	BOOL ResetEvent(HANDLE hEvent);	//to the non-signaled
	BOOL ResetEvent(HANDLE hEvent);	//to the signaled
	�ɹ�ʱ����TRUE��ʧ��ʱ����FALSE
*/


//�ؼ������Ӧ��
#define NUM_THREAD 50
long long g_llNum = 0;
CRITICAL_SECTION cs;
unsigned WINAPI ThreadInc(void* arg);
unsigned WINAPI ThreadDes(void* arg);

//������ʵ���߳�ͬ��
HANDLE hMutex;

//�ź���ʵ���߳�ͬ��
unsigned WINAPI Read(void* arg);
unsigned WINAPI Accu(void* arg);

static int num;
static HANDLE semOne;
static HANDLE semTwo;

//�¼�ʵ���߳�ͬ��
#define STR_LEN 100
unsigned WINAPI NumberOfA(void* arg);
unsigned WINAPI NumberOfOthers(void* arg);

static char str[STR_LEN];
static HANDLE hEvent;



int _tmain(int argc, _TCHAR* argv[])
{
	/*	//�ؼ�����κͻ�����ʵ���߳�ͬ��
	printf("sizeof long long: %d \n", sizeof(long long));

	HANDLE tHandles[NUM_THREAD];

	//�ؼ������ʵ���߳�ͬ��
	//InitializeCriticalSection(&cs);

	//������ʵ���߳�ͬ��
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

	//�ؼ������ʵ���߳�ͬ��
	//DeleteCriticalSection(&cs);

	//������ʵ���߳�ͬ��
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
	//�ؼ������ʵ���߳�ͬ��
	//EnterCriticalSection(&cs);

	//������ʵ���߳�ͬ��
	WaitForSingleObject(hMutex, INFINITE);

	for (int i = 0; i < 10000000; i++)
	{
		g_llNum += 1;
	}

	//�ؼ������ʵ���߳�ͬ��
	//LeaveCriticalSection(&cs);

	//������ʵ���߳�ͬ��
	ReleaseMutex(hMutex);

	puts("ThreadInc end.\n");
	return 0;
}

unsigned WINAPI ThreadDes(void* arg)
{
	//�ؼ������ʵ���߳�ͬ��
	//EnterCriticalSection(&cs);

	//������ʵ���߳�ͬ��
	WaitForSingleObject(hMutex, INFINITE);

	for (int i = 0; i < 10000000; i++)
	{
		g_llNum -= 1;
	}

	//�ؼ������ʵ���߳�ͬ��
	//LeaveCriticalSection(&cs);

	//������ʵ���߳�ͬ��
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