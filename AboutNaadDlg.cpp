// AboutNaadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Naadh.h"
#include "AboutNaadDlg.h"


// CAboutNaadDlg dialog

IMPLEMENT_DYNAMIC(CAboutNaadDlg, CDialog)
CAboutNaadDlg::CAboutNaadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutNaadDlg::IDD, pParent)
{
}

CAboutNaadDlg::~CAboutNaadDlg()
{
}

void CAboutNaadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ABT, m_zoomCtrl);
}


BEGIN_MESSAGE_MAP(CAboutNaadDlg, CDialog)
END_MESSAGE_MAP()


// CAboutNaadDlg message handlers
BOOL CAboutNaadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_AboutText.Format("%.1f", (float)VERSION/10.0);
	m_AboutText = "Naad version " + m_AboutText + ", " + VERMONTH + " \r\n\r\n" + "By\r\nOormi Creations\r\nhttp://oormi.in\r\noormicreations@gmail.com";
				
	m_AboutText += "\r\n\r\n\r\n\r\nCredits :\r\n* Multiline List Control\tCopyright(C) 2006 Dave Calkins (coder1024@gmail.com)\r\n";
	m_AboutText += "* UpdateCheck Class\tCopyright(C) 2003 Alexander Bischofberger (www.bluescreen.de)\r\n";
	m_AboutText += "* Image Support\tJim Dill (jim@jimdill.net) and Tobias Eiseler (tobias.eiseler@sisternicky.com)\r\n";
	m_AboutText += "* WaveBox Class\tZenith__";
	m_AboutText += "\r\n\r\nNaad is a freeware and open source software.";

	m_zoomCtrl.m_ImageName = m_AboutText;
	m_zoomCtrl.m_About = true;

	if(!m_zoomCtrl.LoadFromFile(m_ImageFileName))
	{
		//CDialog::OnCancel();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}
