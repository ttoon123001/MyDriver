
// StartDriverDlg.h: 头文件
//

#pragma once


// CStartDriverDlg 对话框
class CStartDriverDlg : public CDialogEx
{
// 构造
public:
	CStartDriverDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STARTDRIVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnUnload();
	afx_msg void OnBnClickedBtnTest();
	CListCtrl m_listctrl_proc;
	afx_msg void OnBnClickedBtnProctect();
	CString m_csPid;
	HANDLE m_hDevice;
	afx_msg void OnBnClickedBtnConnect();
	void UndateProcessList();
	afx_msg void OnNMRClickListProc(NMHDR *pNMHDR, LRESULT *pResult);
	CMenu m_Menu;
	afx_msg void OnProctectProcess();
};
