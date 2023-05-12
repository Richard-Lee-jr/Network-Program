
// DumpTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DumpTest.h"
#include "DumpTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDumpTestDlg 对话框




CDumpTestDlg::CDumpTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDumpTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDumpTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDumpTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_MADE_BUG, &CDumpTestDlg::OnBnClickedBtnMadeBug)
	ON_BN_CLICKED(IDC_BTN_TEST, &CDumpTestDlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


// CDumpTestDlg 消息处理程序

BOOL CDumpTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDumpTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDumpTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDumpTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDumpTestDlg::OnBnClickedBtnMadeBug()
{
	// TODO: 在此添加控件通知处理程序代码
	CDumpTestDlg* pDlg = nullptr;
	pDlg->UpdateData();
}

#include "fruit.h"
#include "SouthFruit.h"

typedef void (*VF_PTR)();
void PrintVFTable(VF_PTR* table, bool bMutiDrive)
{
	if (table == nullptr)
	{
		return;
	}

	TCHAR chInfo[1024];
	
	for (int i = 0; table[i] != nullptr; i++)
	{
		if (!bMutiDrive)
		{
			if (i == 2)
			{
				break;
			}
		}
		else
		{
			if (i == 3)
			{
				break;
			}
		}
		
		sprintf(chInfo, "[%d] : %p->", i, table[i]);
		::OutputDebugString(chInfo);
		VF_PTR f = table[i];
		f();
	}
}

void CDumpTestDlg::OnBnClickedBtnTest()
{
	// TODO: 在此添加控件通知处理程序代码
	//Fruit fruit;
	//fruit.SetSize(0);
	//fruit.SetPrice(0);
	//SouthFruit southFruit;
	//southFruit.SetDes("des");

	//PrintVFTable((VF_PTR*)*(int*)&fruit);
	//PrintVFTable((VF_PTR*)*(int*)&southFruit);

	//PrintVFTable(*(VF_PTR**)&fruit);
	//PrintVFTable(*(VF_PTR**)&southFruit);
	
	B1 b1;
	B2 b2;
	D d;

	PrintVFTable(*(VF_PTR**)&b1, false);
	PrintVFTable(*(VF_PTR**)&b2, false);

	PrintVFTable(*(VF_PTR**)&d, true);
	//PrintVFTable(*(VF_PTR**)(&d + sizeof(b1)), false);

	int nOffset = sizeof(b1);
	PrintVFTable( (VF_PTR*)*(int*)((char*)&d + nOffset), false );

	return;
}
