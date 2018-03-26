// CompPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Naad.h"
#include "CompPropDlg.h"


// CCompPropDlg dialog

IMPLEMENT_DYNAMIC(CCompPropDlg, CDialog)
CCompPropDlg::CCompPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompPropDlg::IDD, pParent)
{
}

CCompPropDlg::~CCompPropDlg()
{
}

void CCompPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_COMPNAME,m_Name);
	DDX_Text(pDX,IDC_EDIT_COMPOSER,m_Composer);
	DDX_Text(pDX,IDC_EDIT_COMPNOTES,m_Notes);
}


BEGIN_MESSAGE_MAP(CCompPropDlg, CDialog)
END_MESSAGE_MAP()


// CCompPropDlg message handlers
