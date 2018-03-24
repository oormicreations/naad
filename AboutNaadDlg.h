#pragma once
#include "zoomctrl.h"


// CAboutNaadDlg dialog

class CAboutNaadDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutNaadDlg)

public:
	CAboutNaadDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutNaadDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ABT };
	CZoomCtrl m_zoomCtrl;
	CString m_ImageFileName, m_AboutText;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
