#include "stdafx.h"
#include "select.h"
//#include <WinSock2.h>

//#pragma comment(lib, "ws2_32.lib")

//#define BUF_SIZE 1024
/*
	select函数原型
	int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* excepfds, const struct timeval* timeout);
	//nfds：监视对象文件描述符（套接字）的数量
	//readfds：将所有关注“是否存在待读取数据”的文件描述符（套接字）注册到fd_set型变量，并传递其地址值
	//writefds：将所有关注“是否可传输无阻塞数据”的文件描述符（套接字）注册到fd_set型变量，并传递其地址值
	//excepfds：将所有关注“是否发生异常”的文件描述符（套接字）注册到fd_set型变量，并传递其地址值
	//timeout：调用select函数后，为防止陷入无限阻塞状态，传递超时(time-out)信息
	//返回值：发生错误时返回-1，超时返回0.因发生关注的事件返回时，返回大于0的值，该值是发生事件的文件描述符（套接字）

	理解select函数
	select函数的功能和调用顺序
	使用select函数可将多个文件描述符集中到一起统一监视，可监视的项目有：
	①是否存在套接字接收数据？
	②无需阻塞传输数据的套接字有哪些？
	③哪些套接字发生了异常？

	调用顺序如下：
	①设置文件描述符
	②指定监视范围
	③设置超时
	④调用select函数
	⑤查看调用结果

	设置文件描述符
	利用select函数可以同时监视多个文件描述符，但需要将监视的文件描述符集中到一起。
	集中时也需要按照上述的监视项（接收、传输、异常）进行区分

	注意：必须使用fd_set数组集中监视
*/

/*
int selectTest()
{
	fd_set reads, temps;
	FD_ZERO(&reads);

	//0表示标准输入
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
			//可读取
			//判断是否是0（即控制台）的输入
			if (FD_ISSET(0, &temps))
			{
				nRecvLen = recv(0, buf, BUF_SIZE, 0);
				buf[nRecvLen] = 0;
				printf("msg from console: %s\n", buf);
			}
		}
	}

	fputs("任意键继续...", stdout);
	getchar();
	return 0;
}
*/