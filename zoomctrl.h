// zoomctrl.h
//---------------------------------------------------------------------
#ifndef _H_ZoomCtrl
#define _H_ZoomCtrl

#include "scrollhelper.h"

//---------------------------------------------------------------------
class CZoomCtrl : public CWnd
{
public:
						CZoomCtrl();
						~CZoomCtrl();

	double				GetZoomFactor() const		{ return m_zoomFactor; }
	void				SetZoomFactor(double d)		{ m_zoomFactor = d; }
	void				AdjustScrollbars();
	//Loads an image from a file
	BOOL LoadFromFile(CString &szFilePath);
	//Frees the image data
	void FreeData();


	UINT m_Sx, m_Sy, m_Error, m_Page;
	bool m_IsMultiPage, m_ResetPage, m_About;
	CString m_ImageName;

	//{{AFX_MSG(CDendroCtl)
	afx_msg void		OnPaint();
	afx_msg void		OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL		OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT				WindowProc(UINT mess, WPARAM wParam, LPARAM lParam);
	afx_msg int			OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);

protected:
	virtual void		Draw(CDC *pDC);
	void				PrepDC(CDC *pDC, const CRect& rVirt, const CRect& rScreen);

	CRect				m_rVirt, m_rScreen;
	CScrollHelper		m_scrollHelper;
	double				m_zoomFactor;
private:

	//Internal image stream buffer
	IStream* m_pStream;

	//Control flag if a pic is loaded
	BOOL m_bIsPicLoaded;

	//GDI Plus Token
	ULONG_PTR m_gdiplusToken;


};
//---------------------------------------------------------------------
#endif // _H_ZoomCtrl
