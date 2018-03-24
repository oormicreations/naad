#pragma once


// CCompPropDlg dialog

class CCompPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompPropDlg)

public:
	CCompPropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCompPropDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROPCOMP };

	CString m_Name, m_Composer, m_Notes;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
