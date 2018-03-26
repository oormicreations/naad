// LoopPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Naad.h"
#include "LoopPropDlg.h"


// CLoopPropDlg dialog

IMPLEMENT_DYNAMIC(CLoopPropDlg, CDialog)
CLoopPropDlg::CLoopPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoopPropDlg::IDD, pParent)
	, m_Name(_T(""))
	, m_Repeat(1)
	, m_Bpm(DEFAULT_BPM)
{
}

CLoopPropDlg::~CLoopPropDlg()
{
}

void CLoopPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MATRA, m_ComboMatra);
	DDX_Control(pDX, IDC_COMBO_ACC, m_ComboAcc);
	DDX_Text(pDX, IDC_EDIT_REPEAT, m_Repeat);
	DDX_Text(pDX, IDC_EDIT_LOOPNAME, m_Name);
	DDX_Text(pDX, IDC_EDIT_LOOPNOTES, m_Note);
	DDX_Text(pDX, IDC_EDIT_DELAY, m_Bpm);
	//DDX_Check(pDX, IDC_CHECK_B2B, m_B2B);
}


BEGIN_MESSAGE_MAP(CLoopPropDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_MATRA, OnCbnSelchangeComboMatra)
	ON_CBN_SELCHANGE(IDC_COMBO_ACC, OnCbnSelchangeComboAcc)
	ON_BN_CLICKED(IDC_CHECK_B2B, OnBnClickedCheckB2b)
END_MESSAGE_MAP()


// CLoopPropDlg message handlers
BOOL CLoopPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetDlgItemTextA(IDC_EDIT_MAATRAS, m_MatraCountStr);
	m_ComboMatra.SetCurSel(m_Matra-1);
	m_ComboAcc.SetCurSel(m_Acc);
	CButton * chk = (CButton*)this->GetDlgItem(IDC_CHECK_B2B);
	//chk->SetCheck(m_B2B);
	//OnBnClickedCheckB2b();
	return TRUE;
}
void CLoopPropDlg::OnCbnSelchangeComboMatra()
{
	m_Matra = m_ComboMatra.GetCurSel() + 1;
}

void CLoopPropDlg::OnCbnSelchangeComboAcc()
{
	m_Acc = m_ComboAcc.GetCurSel();
}

void CLoopPropDlg::OnOK()
{
	CDialog::OnOK();
}

void CLoopPropDlg::OnBnClickedCheckB2b()
{
	//this->GetDlgItem(IDC_EDIT_DELAY)->EnableWindow(!this->IsDlgButtonChecked(IDC_CHECK_B2B));
}
