#pragma once
#include "afxwin.h"
#include "Resource.h"

// CLoopPropDlg dialog

class CLoopPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoopPropDlg)

public:
	CLoopPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoopPropDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROPLOOP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
//----------------------------------------------------------------------------------------
	CString		m_Name, m_Note, m_MatraCountStr;
	CComboBox	m_ComboMatra;
	CComboBox	m_ComboAcc;
	UINT		m_Repeat, m_Acc, m_Matra, m_Bpm;

//----------------------------------------------------------------------------------------
	afx_msg void OnCbnSelchangeComboMatra();
	afx_msg void OnCbnSelchangeComboAcc();
	afx_msg void OnBnClickedCheckB2b();
};
