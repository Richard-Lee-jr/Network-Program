#include "stdafx.h"
#include "select.h"
//#include <WinSock2.h>

//#pragma comment(lib, "ws2_32.lib")

//#define BUF_SIZE 1024
/*
	select����ԭ��
	int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* excepfds, const struct timeval* timeout);
	//nfds�����Ӷ����ļ����������׽��֣�������
	//readfds�������й�ע���Ƿ���ڴ���ȡ���ݡ����ļ����������׽��֣�ע�ᵽfd_set�ͱ��������������ֵַ
	//writefds�������й�ע���Ƿ�ɴ������������ݡ����ļ����������׽��֣�ע�ᵽfd_set�ͱ��������������ֵַ
	//excepfds�������й�ע���Ƿ����쳣�����ļ����������׽��֣�ע�ᵽfd_set�ͱ��������������ֵַ
	//timeout������select������Ϊ��ֹ������������״̬�����ݳ�ʱ(time-out)��Ϣ
	//����ֵ����������ʱ����-1����ʱ����0.������ע���¼�����ʱ�����ش���0��ֵ����ֵ�Ƿ����¼����ļ����������׽��֣�

	���select����
	select�����Ĺ��ܺ͵���˳��
	ʹ��select�����ɽ�����ļ����������е�һ��ͳһ���ӣ��ɼ��ӵ���Ŀ�У�
	���Ƿ�����׽��ֽ������ݣ�
	�����������������ݵ��׽�������Щ��
	����Щ�׽��ַ������쳣��

	����˳�����£�
	�������ļ�������
	��ָ�����ӷ�Χ
	�����ó�ʱ
	�ܵ���select����
	�ݲ鿴���ý��

	�����ļ�������
	����select��������ͬʱ���Ӷ���ļ�������������Ҫ�����ӵ��ļ����������е�һ��
	����ʱҲ��Ҫ���������ļ�������ա����䡢�쳣����������

	ע�⣺����ʹ��fd_set���鼯�м���
*/

/*
int selectTest()
{
	fd_set reads, temps;
	FD_ZERO(&reads);

	//0��ʾ��׼����
	FD_SET(stdin, &reads);

	timeval timeout;
	int nSelRet = -1;

	char buf[BUF_SIZE];
	int nRecvLen = 0;
	while (true)
	{
		temps = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		
		nSelRet = select(1, &temps, nullptr, nullptr, &timeout);
		if (nSelRet == -1)
		{
			puts("select() error!");
			break;
		}
		else if (nSelRet == 0)
		{
			puts("time out!");
		}
		else
		{
			//�ɶ�ȡ
			//�ж��Ƿ���0��������̨��������
			if (FD_ISSET(0, &temps))
			{
				nRecvLen = recv(0, buf, BUF_SIZE, 0);
				buf[nRecvLen] = 0;
				printf("msg from console: %s\n", buf);
			}
		}
	}

	fputs("���������...", stdout);
	getchar();
	return 0;
}
*/