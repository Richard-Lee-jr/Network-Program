
// DumpTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDumpTestApp:
// �йش����ʵ�֣������ DumpTest.cpp
//

class CDumpTestApp : public CWinApp
{
public:
	CDumpTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDumpTestApp theApp;