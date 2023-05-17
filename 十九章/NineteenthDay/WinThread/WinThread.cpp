// WinThread.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

/*
	�̰߳�ȫ��׼C����,ǩ�����£�
	#include <process.h>

	uintptr_t _beginthreadex(
		void* security,
		unsigned stack_size,
		unsigned (* start_address)(void*),
		void* arglist,
		unsigned initflag,
		unsigned* thrdaddr);
	�ɹ�ʱ�����߳̾����ʧ��ʱ����0
	
	security: �̰߳�ȫ�����Ϣ��ʹ��Ĭ������ʱ����NULL
	stack_size: Ҫ������̵߳�ջ��С������0ʱ����Ĭ�ϴ�С��ջ
	start_address: �����̵߳�main������Ϣ
	arglist: �����߳�main�����Ĵ��ݵĲ�����Ϣ
	initflag: ����ָ���̴߳��������Ϊ������0ʱ���̴߳��������������ִ��״̬
	thrdaddr: ���ڱ����߳�ID�ı�����ֵַ

	������ں˶����Լ�ID֮��Ĺ�ϵ
	�߳�Ҳ���ڲ���ϵͳ������ں���Դ����˻�������ں˶���Ĵ�������Ϊ�������ں˶�������ؾ����
	����ͨ����������ں˶���ͨ���ں˶�����������̡߳�����ͨ����������̡߳�
	���߳�ID����ʲô��? �����_beginthreadex���������һ��������������߳�ID��
	���������ֵ�ڲ�ͬ�Ľ����п����ظ����֣����߳�ID��ȴ�����ڷ�Χ�ڲ����ظ����֡�
	�����߳�ID�������ֲ���ϵͳ�����������̡߳�


	WaitForSingleObject �� WaitForMultipleObjects
	#include <windows.h>
	DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

	hHandle: �鿴״̬���ں˶�����
	dwMilliseconds: ��msΪ��λָ����ʱ������INFINTEʱ�������᷵�أ���������������ֱ���ں˶��󱻴���
	����signaled״̬����WAIT_OBJECT_0����ʱ����WAIT_TIMEOUT������ʧ��ʱ����WAIT_FAILED��

	DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* plHandles, BOOL bWaitAll, DWORD dwMilliseconds);
	nCount:  ����֤���ں˶�����
	lpHandles: �����ں˶������������ֵַ
	bWaitAll: ���ΪTRUE�����������ݶ���ȫ����Ϊsignaledʱ���أ����ΪFALSE����ֻҪ��1����֤�����״̬��Ϊsignaled�ͻ᷵��
	dwMilliseconds: ��msΪ��λָ����ʱ������INFINITEʱ�������᷵�أ�ֱ���ں˶����Ϊsignaled״̬
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

	//ʹ��WaitForSingleObject�����ȴ��߳̽���
	DWORD dwRet;
	if ( (dwRet = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED )
	{
		puts("thread wait error!");
		return -1;
	}

	printf("thread wait result: %s \n", (dwRet == WAIT_OBJECT_0) ? "�߳���������" : "��ʱ");
	*/

	printf("sizeof long long: %d \n", sizeof(long long));
	//���￪NUM_THREAD�߳�, ����NUM_THREAD/2���̶߳�ȫ�ֱ���ָ��+1������NUM_THREAD/2���̶߳�ȫ�ֱ���ִ�м�1��������Ҫ������Ч��Ϊֵ�������仯
	//���ﲻ��ﵽԤ��Ч������ʹ���߳�ͬ������
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