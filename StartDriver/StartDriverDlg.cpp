
// StartDriverDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "StartDriver.h"
#include "StartDriverDlg.h"
#include "afxdialogex.h"


#include <Windows.h>
#include <winsvc.h>
#include <winioctl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//#define DRIVER_NAME "ntmodeldrv"
//#define DRIVER_PATH ".\\ntmodeldrv.sys"

#define IOCTRL_BASE 0x800

#define MYIOCTRL_CODE(i) \
	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTRL_BASE+i, METHOD_BUFFERED,FILE_ANY_ACCESS)

#define CTL_HELLO MYIOCTRL_CODE(0)
#define CTL_PRINT MYIOCTRL_CODE(1)
#define CTL_BYE MYIOCTRL_CODE(2)





#define IS_USE_OUTPUT_DEBUG_PRINT   1

#if  IS_USE_OUTPUT_DEBUG_PRINT 

#define  OUTPUT_DEBUG_PRINTF(str)  OutputDebugPrintf(str)
void OutputDebugPrintf(const char * strOutputString, ...)
{
#define PUT_PUT_DEBUG_BUF_LEN   1024
	char strBuffer[PUT_PUT_DEBUG_BUF_LEN] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);  //_vsnprintf_s  _vsnprintf
	//vsprintf(strBuffer,strOutputString,vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);  //OutputDebugString    // OutputDebugStringW

}
#else 
#define  OUTPUT_DEBUG_PRINTF(str) 
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CStartDriverDlg 对话框



CStartDriverDlg::CStartDriverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STARTDRIVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStartDriverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROC, m_listctrl_proc);
}

BEGIN_MESSAGE_MAP(CStartDriverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOAD, &CStartDriverDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_UNLOAD, &CStartDriverDlg::OnBnClickedBtnUnload)
	ON_BN_CLICKED(IDC_BTN_TEST, &CStartDriverDlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


// CStartDriverDlg 消息处理程序

BOOL CStartDriverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_listctrl_proc.InsertColumn(0, L"进程名");
	m_listctrl_proc.InsertColumn(1, L"PID");
	m_listctrl_proc.InsertColumn(2, L"路径");

	m_listctrl_proc.SetColumnWidth(0, 120);
	m_listctrl_proc.SetColumnWidth(1, 80);
	m_listctrl_proc.SetColumnWidth(2, 200);


	m_listctrl_proc.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CStartDriverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStartDriverDlg::OnPaint()
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
HCURSOR CStartDriverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//装载NT驱动程序
BOOL LoadDriver(CString lpszDriverName, CString lpszDriverPath)
{

	//char szDriverImagePath[256] = "D:\\DriverTest\\ntmodelDrv.sys";
	WCHAR szDriverImagePath[256] = { 0 };
	//得到完整的驱动路径
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄

	//打开服务控制管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		//OpenSCManager失败

		OutputDebugPrintf("OpenSCManager() Failed %d ! \n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		////OpenSCManager成功
		OutputDebugPrintf("OpenSCManager() ok ! \n");
	}

	//创建驱动所对应的服务
	hServiceDDK = CreateService(hServiceMgr,
		lpszDriverName, //驱动程序的在注册表中的名字  
		lpszDriverName, // 注册表驱动程序的 DisplayName 值  
		SERVICE_ALL_ACCESS, // 加载驱动程序的访问权限  
		SERVICE_FILE_SYSTEM_DRIVER,// 表示加载的服务是驱动程序  
		SERVICE_DEMAND_START, // 注册表驱动程序的 Start 值  
		SERVICE_ERROR_IGNORE, // 注册表驱动程序的 ErrorControl 值  
		szDriverImagePath, // 注册表驱动程序的 ImagePath 值  
		NULL,  //GroupOrder HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\GroupOrderList
		NULL,
		NULL,
		NULL,
		NULL);

	DWORD dwRtn;
	//判断服务是否失败
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			//由于其他原因创建服务失败
			OutputDebugPrintf("CrateService() Failed %d ! \n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			//服务创建失败，是由于服务已经创立过
			OutputDebugString(L"CrateService() Failed Service is ERROR_IO_PENDING or ERROR_SERVICE_EXISTS! \n");
		}

		// 驱动程序已经加载，只需要打开  
		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			//如果打开服务也失败，则意味错误
			dwRtn = GetLastError();
			OutputDebugPrintf("OpenService() Failed %d ! \n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			OutputDebugPrintf("OpenService() ok ! \n");
		}
	}
	else
	{
		OutputDebugPrintf("CrateService() ok ! \n");
	}

	//开启此项服务
	bRet = StartService(hServiceDDK, NULL, NULL);
	if (!bRet)
	{
		DWORD dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			OutputDebugPrintf("StartService() Failed %d ! \n" ,dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				//设备被挂住
				OutputDebugPrintf("StartService() Failed ERROR_IO_PENDING ! \n");
				bRet = FALSE;
				goto BeforeLeave;
			}
			else
			{
				//服务已经开启
				OutputDebugPrintf("StartService() Failed ERROR_SERVICE_ALREADY_RUNNING ! \n");
				bRet = TRUE;
				goto BeforeLeave;
			}
		}
	}
	bRet = TRUE;
	//离开前关闭句柄
BeforeLeave:
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}


//卸载驱动程序  
BOOL UnloadDriver(CString szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄
	SERVICE_STATUS SvrSta;
	//打开SCM管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		//带开SCM管理器失败
		OutputDebugPrintf("OpenSCManager() Failed %d ! \n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		//带开SCM管理器失败成功
		OutputDebugPrintf("OpenSCManager() ok ! \n");
	}
	//打开驱动所对应的服务
	hServiceDDK = OpenService(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);

	if (hServiceDDK == NULL)
	{
		//打开驱动所对应的服务失败
		OutputDebugPrintf("OpenService() Failed %d ! \n" ,GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		OutputDebugString(L"OpenService() ok ! \n");
	}
	//停止驱动程序，如果停止失败，只有重新启动才能，再动态加载。  
	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta))
	{
		OutputDebugPrintf("ControlService() Failed %d !\n", GetLastError());
	}
	else
	{
		//打开驱动所对应的失败
		OutputDebugPrintf("ControlService() ok !\n");
	}


	//动态卸载驱动程序。  

	if (!DeleteService(hServiceDDK))
	{
		//卸载失败
		OutputDebugPrintf("DeleteSrevice() Failed %d !\n", GetLastError());
	}
	else
	{
		//卸载成功
		OutputDebugPrintf("DelServer:deleteSrevice() ok !\n");
	}

	bRet = TRUE;
BeforeLeave:
	//离开前关闭打开的句柄
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}


void TestDriver()
{
	//测试驱动程序  
	HANDLE hDevice = CreateFile(L"\\\\.\\ProtectProcess_001",
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		OutputDebugPrintf("Create Device ok ! \n");
	}
	else
	{
		OutputDebugPrintf("Create Device Failed %d ! \n", GetLastError());
		return;
	}
	CHAR bufRead[1024] = { 0 };
	WCHAR bufWrite[1024] = L"Hello, world";

	DWORD dwRead = 0;
	DWORD dwWrite = 0;
	MessageBox(0, L"Read", 0, 0);
	ReadFile(hDevice, bufRead, 1024, &dwRead, NULL);
	OutputDebugString(L"Read done!:%ws\n");
	OutputDebugString(L"Please press any key to write\n");
	MessageBox(0,L"Write",0,0);
	WriteFile(hDevice, bufWrite, (wcslen(bufWrite) + 1) * sizeof(WCHAR), &dwWrite, NULL);

	OutputDebugString(L"Write done!\n");

	OutputDebugString(L"Please press any key to deviceiocontrol\n");
	
	CHAR bufInput[1024] = "Hello, world";
	CHAR bufOutput[1024] = { 0 };
	DWORD dwRet = 0;

	WCHAR bufFileInput[1024] = L"c:\\docs\\hi.txt";

	OutputDebugString(L"Please press any key to send PRINT\n");
	MessageBox(0, L"PRINT", 0, 0);
	DeviceIoControl(hDevice,
		CTL_PRINT,
		bufFileInput,
		sizeof(bufFileInput),
		bufOutput,
		sizeof(bufOutput),
		&dwRet,
		NULL);
	OutputDebugString(L"Please press any key to send HELLO\n");
	MessageBox(0, L"HELLO", 0, 0);
	DeviceIoControl(hDevice,
		CTL_HELLO,
		NULL,
		0,
		NULL,
		0,
		&dwRet,
		NULL);
	OutputDebugString(L"Please press any key to send BYE\n");
	MessageBox(0, L"BYE", 0, 0);
	DeviceIoControl(hDevice,
		CTL_BYE,
		NULL,
		0,
		NULL,
		0,
		&dwRet,
		NULL);
	OutputDebugString(L"DeviceIoControl done!\n");
	CloseHandle(hDevice);
}





void CStartDriverDlg::OnBnClickedBtnLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	WCHAR pFileName[MAX_PATH];
	CString csFileName;
	GetCurrentDirectory(MAX_PATH, pFileName);
	csFileName.Format(L"%s%s", pFileName, L"\\ProtectProcess.sys");
	LoadDriver(L"ProtectProcess", csFileName);
}


void CStartDriverDlg::OnBnClickedBtnUnload()
{
	// TODO: 在此添加控件通知处理程序代码
	UnloadDriver(L"ProtectProcess");
}


void CStartDriverDlg::OnBnClickedBtnTest()
{
	// TODO: 在此添加控件通知处理程序代码

	TestDriver();

}
