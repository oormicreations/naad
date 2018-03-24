//	zoomctrl.cpp
//---------------------------------------------------------------------
/*
	Child control with zoom and scroll

	Jim Dill [jim@jimdill.net]
	v 1.0, Mar 25 2008
*/
//---------------------------------------------------------------------
#include "stdafx.h"
#include "zoomctrl.h"
#include <GdiPlus.h>
using namespace Gdiplus;
//Macro to release COM Components

#ifdef SAFE_RELEASE
#undef SAFE_RELEASE
#endif
#define SAFE_RELEASE(x) do{\
							if((x) != NULL)\
							{\
								while((x)->Release() != 0);\
								(x) = NULL;\
							}\
						}while(0)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------
CZoomCtrl::CZoomCtrl()
{
    m_scrollHelper.AttachWnd(this);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_pStream		= NULL;
	m_zoomFactor	= 1.0;
	m_gdiplusToken	= 0;
	m_Error			= 0;
	m_Sx			= 0;
	m_Sy			= 0;
	m_Page			= 0;
	m_bIsPicLoaded	= FALSE;
	m_ResetPage		= false;
	m_IsMultiPage	= false;
	m_About			= false;
}
//---------------------------------------------------------------------
CZoomCtrl::~CZoomCtrl()
{
}
//---------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CZoomCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEACTIVATE()
    ON_WM_MOUSEWHEEL()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//---------------------------------------------------------------------
void CZoomCtrl::Draw(CDC *pDC)
{
	// fill background
	CRect rClient;
	GetClientRect(rClient);
	COLORREF backgrColor = RGB(80,80,80);
	pDC->FillRect(&rClient, &CBrush(backgrColor));


	// define virtual drawing space -- change this to suit
	// if drawing must not distort, then create a rect same shape as image
	Image image(m_pStream);
	m_Sx = image.GetWidth();
	m_Sy = image.GetHeight();
	rClient.right  = (LONG)((double)m_Sx * m_zoomFactor);
	rClient.bottom = (LONG)((double)m_Sy * m_zoomFactor);
	CRect rVirt(0, 0, rClient.right, rClient.bottom);

	PrepDC(pDC, rVirt, rClient);

	// do your drawing here, in virtual coords
	Graphics graphics(pDC->GetSafeHdc());
	//graphics.Clear(Color::White);
	////Get control measures
	////RECT rc;
	////this->GetClientRect(&rc);
	////CRect rc(0, 0, 3000, 2800);

	//// scale and draw the picture
	int nHeight = (rVirt.bottom-rVirt.top);
	int nWidth  = (rVirt.right-rVirt.left);
	//double XScale = (double) nWidth / (double)image.GetWidth();
	//double YScale = (double) nHeight / (double)image.GetHeight();
	//double Scale = 1.0;
	//if ( XScale < YScale )
	//Scale = XScale;
	//else
	//Scale = YScale;

	//nHeight = image.GetHeight() * Scale;
	//nWidth = image.GetWidth() * Scale;


	if(m_IsMultiPage)
	{
		GUID   pageGuid = FrameDimensionPage;
		UINT pc = image.GetFrameCount(&pageGuid);
		if(m_Page > pc) {m_Page = 1; m_ResetPage = true;}
		image.SelectActiveFrame(&pageGuid, m_Page-1);
	}

	graphics.DrawImage(&image, (INT)rVirt.left, (INT)rVirt.top, (INT)nWidth, (INT) nHeight );

	if(m_Error>0)
	{
		FontFamily  fontFamily(L"Century Gothic");
		Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);
		PointF      pointF(120.0f, 230.0f);
		SolidBrush  solidBrush(Color(255, 220, 0, 0));
		WCHAR*		errstr;

		if(m_Error==1) errstr = L"Error : Image file was not found";
		if(m_Error==2) errstr = L"Error : Image file could not be loaded";
		if(m_Error==3) errstr = L"Error : Image file stream creation failed";
		graphics.DrawString(errstr, -1, &font, pointF, &solidBrush);
		pointF.X = 80.f;
		pointF.Y = 280.f;
		Font font2(&fontFamily,12, FontStyleItalic, UnitPixel);
		solidBrush.SetColor(Color(0,0,0));
		errstr = L"Unknown Error";
		graphics.DrawString(errstr, -1, &font2, pointF, &solidBrush);
		pointF.X = 80.f;
		pointF.Y = 400.f;
		LPTSTR lpStr = m_ImageName.GetBuffer( m_ImageName.GetLength() ); 
		int nLen = MultiByteToWideChar(CP_ACP, 0,lpStr, -1, NULL, NULL);
		LPWSTR lpszW = new WCHAR[nLen];
		MultiByteToWideChar(CP_ACP, 0, lpStr, -1, lpszW, nLen);
		
		graphics.DrawString(lpszW, -1, &font2, pointF, &solidBrush);
		delete [] lpszW;
	}

	if(m_About)
	{
		FontFamily  fontFamily(L"Century Gothic");
		Font        font(&fontFamily, 11, FontStyleRegular, UnitPixel);
		PointF      pointF(10.0f, 10.0f);
		SolidBrush  solidBrush(Color(255, 255, 230, 55));

		LPTSTR lpStr = m_ImageName.GetBuffer( m_ImageName.GetLength() ); 
		int nLen = MultiByteToWideChar(CP_ACP, 0,lpStr, -1, NULL, NULL);
		LPWSTR lpszW = new WCHAR[nLen];
		MultiByteToWideChar(CP_ACP, 0, lpStr, -1, lpszW, nLen);
		
		graphics.DrawString(lpszW, -1, &font, pointF, &solidBrush);
		delete [] lpszW;
	}
	

	// here's our sample
	//CRect rTest(2000, 2200, 3000, 2800);
	//CPen penBrown(PS_SOLID, 0, RGB(192,110,0));
	//CPen penGreen(PS_SOLID, 0, RGB(0,192,0));
	//CPen* pOldPen = pDC->SelectObject(&penGreen);
	//pDC->Ellipse(&rTest);

	//pDC->SelectObject(&penBrown);
	//pDC->MoveTo(0, 0);
	//pDC->LineTo(5000, 5000);
	//pDC->MoveTo(5000, 0);
	//pDC->LineTo(0, 5000);

	//pDC->SelectObject(pOldPen);
}
//---------------------------------------------------------------------
void CZoomCtrl::OnPaint()
{
	CPaintDC dc(this);
	/*if(m_bIsPicLoaded) */Draw(&dc);
}
//---------------------------------------------------------------------
void CZoomCtrl::PrepDC(CDC *pDC, const CRect& rVirt, const CRect& rScreen)
{
	// maps virtual into client
	m_rVirt = rVirt;
	m_rScreen = rScreen;

	//pDC->IntersectClipRect(&rScreen);

	//pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(rVirt.Width(), rVirt.Height());

	//LONG wid = (LONG)(m_zoomFactor * (double)rVirt.Width());
	//LONG hgt = (LONG)(m_zoomFactor * (double)rVirt.Height());
	LONG wid = (LONG)(/*m_zoomFactor * (double)*/rScreen.Width() );
	LONG hgt = (LONG)(/*m_zoomFactor * (double)*/rScreen.Height());
	pDC->SetViewportExt(wid, hgt);

	CSize scrollPos = m_scrollHelper.GetScrollPos();
	pDC->SetViewportOrg(-scrollPos.cx, -scrollPos.cy);
}
//---------------------------------------------------------------------
void CZoomCtrl::AdjustScrollbars()
{
	int xMax = (int)((double)m_rScreen.Width() * m_zoomFactor);
	int yMax = (int)((double)m_rScreen.Height() * m_zoomFactor);
	m_scrollHelper.SetPageSize(m_rScreen.Width(), m_rScreen.Height());
	m_scrollHelper.SetDisplaySize(xMax, yMax);
}
//---------------------------------------------------------------------
void CZoomCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    m_scrollHelper.OnHScroll(nSBCode, nPos, pScrollBar);
	Invalidate();
}
//---------------------------------------------------------------------
void CZoomCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    m_scrollHelper.OnVScroll(nSBCode, nPos, pScrollBar);
	Invalidate();
}
//---------------------------------------------------------------------
BOOL CZoomCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    return m_scrollHelper.OnMouseWheel(nFlags, zDelta, pt);
}
//---------------------------------------------------------------------
LRESULT CZoomCtrl::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam) 
{
	// necessary for getting h/vscroll msgs
    if (msg == WM_NCHITTEST || msg == WM_NCLBUTTONDOWN || msg == WM_NCLBUTTONDBLCLK) 
        return ::DefWindowProc(m_hWnd, msg, wParam, lParam); 
    return CWnd::WindowProc(msg, wParam, lParam); 
} 
//---------------------------------------------------------------------
int CZoomCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
     // necessary for getting mousewheel messages
	int status = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
    SetFocus();
	return status;
}
//---------------------------------------------------------------------
BOOL CZoomCtrl::OnEraseBkgnd(CDC* pDC)
{
	// prevent erase to reduce flashing
	// for smoothest resize, also set clipchildren style in parent dialog
    return TRUE;
}
//---------------------------------------------------------------------

BOOL CZoomCtrl::LoadFromFile(CString &szFilePath)
{
	//Set success error state
	SetLastError(ERROR_SUCCESS);
	FreeData();

	//Allocate stream
	DWORD dwResult = CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	if(dwResult != S_OK)
	{
		SetLastError(dwResult);
		return FALSE;
	}

	//Open the specified file
	CFile cFile;
	CFileException cFileException;
	if(!cFile.Open(szFilePath, CStdioFile::modeRead | CStdioFile::typeBinary, &cFileException))
	{
		SetLastError(cFileException.m_lOsError);
		SAFE_RELEASE(m_pStream);
		m_Error = 1;//"Failed to load image file : " + szFilePath;
		return FALSE;
	}

	//Copy the specified file's content to the stream
	BYTE pBuffer[1024] = {0};
	while(UINT dwRead = cFile.Read(pBuffer, 1024))
	{
		dwResult = m_pStream->Write(pBuffer, dwRead, NULL);
		if(dwResult != S_OK)
		{
			SetLastError(dwResult);
			SAFE_RELEASE(m_pStream);
			cFile.Close();
			m_Error = 3;
			return FALSE;
		}
	}

	//Close the file
	cFile.Close();

	//Mark as Loaded
	//m_bIsPicLoaded = TRUE;

	Invalidate();
	RedrawWindow();

	return TRUE;
}

void CZoomCtrl::FreeData()
{
	m_bIsPicLoaded = FALSE;
	SAFE_RELEASE(m_pStream);
}
