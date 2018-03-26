/*
 * CMultilineList custom control
 * Copyright (C) 2006 Dave Calkins (coder1024@gmail.com)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the
 *
 *   Free Software Foundation, Inc.
 *   59 Temple Place, Suite 330
 *   Boston, MA 02111-1307 USA 
 *
 */

#include "stdafx.h"
#include "MultilineList.h"

using namespace std;

// Win32 class name used for the window; if you're using the VS dialog editor you need
// to put this string in the Class property of the custom control
#define MULTILINELIST_CLASSNAME _T("CMultilineList")

// width of the internal borders used between cells/cols/rows
#define GRID_WIDTH              1

// spacing inside cells between the text and the cell borders
#define INNER_PADDING           2

// control IDs for child controls
#define CHILD_ID_HEADERCTRL     101
#define CHILD_ID_SCROLLBAR      102
#define CHILD_ID_SCROLLBARHORZ  103

// header control height is set to be the font height plus the below
#define HEADERCTRL_HEIGHT_EXTRA 6

// # pixels to scroll horizontally when using arrows
#define HORZSCROLL_PIXELS       25

IMPLEMENT_DYNAMIC(CMultilineList, CWnd)
BEGIN_MESSAGE_MAP(CMultilineList, CWnd)
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_VSCROLL()
   ON_WM_MOUSEWHEEL()
   ON_WM_HSCROLL()
   ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
//                                  PUBLIC
////////////////////////////////////////////////////////////////////////////////

CMultilineList::CMultilineList()
: m_nCols(0),
  m_nRows(0),
  m_viewYPos(0),
  m_viewXPos(0),
  m_markYPos(0),
  m_markXPos(0),
  m_curSelRow(-1),
  m_IsMarkable(false),
  m_nHighlightedRow(0),
  m_sel(false),
  m_selcol(0),
  m_selrow(0)
{
   RegisterWindowClass();
}

CMultilineList::~CMultilineList()
{
}

BOOL CMultilineList::Create(CWnd* pParentWnd, const RECT& rect, UINT nID,
                            DWORD dwStyle /*=WS_VISIBLE*/)
{
	return CWnd::Create(MULTILINELIST_CLASSNAME, _T(""), dwStyle, rect,
      pParentWnd, nID);
}

void CMultilineList::SetSize(int nCols, int nRows)
{
   ASSERT(nCols >= 0);
   ASSERT(nRows >= 0);

   // if no change, do nothing
   if ((nCols == m_nCols) &&
       (nRows == m_nRows))
   {
      return;
   }

   // if the # cols or # rows is being reduced
   if ((nCols < m_nCols) ||
       (nRows < m_nRows))
   {
      // walk through all cells
      map<pair<int,int>,Cell>::iterator i;
      for (i = m_cells.begin(); i != m_cells.end(); )
      {
         int col = i->first.first;
         int row = i->first.second;

         // remove any cells outside the new dimensions
         if ((col >= nCols) ||
             (row >= nRows))
         {
            i = m_cells.erase(i);
         }
         else
         {
            ++i;
         }
      }
   }

   // if the # cols is being reduced
   if (nCols < m_nCols)
   {
      // walk through all columns
      map<int,Column>::iterator j;
      for (j = m_columns.begin(); j != m_columns.end(); )
      {
         int col = j->first;

         // remove any columns outside the new dimensions
         if (col >= nCols)
         {
            j = m_columns.erase(j);
         }
         else
         {
            ++j;
         }
      }

      // invalidate all row heights
      m_rowHeights.clear();
   }

   // if the # rows is being reduced but # cols is not being reduced
   // (since if the # cols was reduced all row heights would have been
   // invalidated anyway)
   if ((nRows < m_nRows) &&
       (nCols >= m_nCols))
   {
      // walk through the calculated row heights
      map<int,int>::iterator k;
      for (k = m_rowHeights.begin(); k != m_rowHeights.end(); )
      {
         int row = k->first;

         // remove (invalidate) calculated row heights corresponding to rows which are outside the new dimensions
         if (row >= nRows)
         {
            k = m_rowHeights.erase(k);
         }
         else
         {
            ++k;
         }
      }
   }

   // store new size
   m_nCols = nCols;
   m_nRows = nRows;

   // update
   Invalidate(FALSE);
   CalculateRowHeights();
   UpdateChildControls();
}

void CMultilineList::GetSize(int & nCols, int & nRows)
{
   // return current size of the list
   nCols = m_nCols;
   nRows = m_nRows;
}

void CMultilineList::SetColHeading(int col, LPCTSTR heading)
{
   ASSERT(col >= 0);
   ASSERT(col < m_nCols);
   ASSERT(heading != NULL);

   // use existing column object (if there is one) or a new one
   Column column;
   map<int,Column>::iterator i = m_columns.find(col);
   if (i != m_columns.end())
   {
      column = i->second;

      // abort if no change
      if (column.m_heading == CString(heading))
         return;
   }

   // set column heading
   column.m_heading = heading;

   // store the column object back in the map
   m_columns[col] = column;

   // update
   Invalidate(FALSE);
   UpdateChildControls();
}

void CMultilineList::SetColWidth(int col, int width)
{
   ASSERT(col >= 0);
   ASSERT(col < m_nCols);
   ASSERT(width >= 0);

   // use existing column object (if there is one) or use default
   Column column;
   map<int,Column>::iterator i = m_columns.find(col);
   if (i != m_columns.end())
   {
      column = i->second;

      // abort if no change
      if (column.m_width == width)
         return;
   }

   // set column width
   column.m_width = width;

   // store the column object back in the map
   m_columns[col] = column;

   // invalidate all row heights
   m_rowHeights.clear();

   // update
   Invalidate(FALSE);
   CalculateRowHeights();
   UpdateChildControls();
}

void CMultilineList::SetCellText(int col, int row, LPCTSTR text, int align, COLORREF color)
{
   ASSERT(col >= 0);
   ASSERT(row >= 0);
   ASSERT(col < m_nCols);
   ASSERT(row < m_nRows);
   ASSERT(text != NULL);

   // use existing cell object (if there is one) or a new one
   Cell cell;
   pair<int,int> coord = make_pair(col,row);
   map<pair<int,int>,Cell>::iterator i = m_cells.find(coord);
   if (i != m_cells.end())
   {
      cell = i->second;

      // abort if no change
      if (cell.m_text == CString(text))
         return;
   }

   //set color
	cell.m_TextColor = color;

   // set cell text
   cell.m_text = text;
   cell.m_align = align;

   // store the cell object back in the map
   m_cells[coord] = cell;

   // invalidate this row's height
   m_rowHeights.erase(row);

   // update
   Invalidate(FALSE);
   CalculateRowHeights();
   UpdateChildControls();
}

void CMultilineList::SetCellMark(int col, int row, BOOL mark)
{
   ASSERT(col >= 0);
   ASSERT(row >= 0);
   ASSERT(col < m_nCols);
   ASSERT(row < m_nRows);
   // use existing cell object (if there is one) or a new one
   Cell cell;
   pair<int,int> coord = make_pair(col,row);
   map<pair<int,int>,Cell>::iterator i = m_cells.find(coord);
   if (i != m_cells.end())
   {
      cell = i->second;
      if (cell.m_marked == mark) return;	// abort if no change
   }
   // set cell mark
	if(!cell.m_disabled) 
	{
		cell.m_marked = mark;
		m_cells[coord] = cell;	// store the cell object back in the map
	}
   // update
   Invalidate(FALSE);
   CalculateRowHeights();
   UpdateChildControls();
}


void CMultilineList::SetCellBg(int col, int row, BOOL mark)
{
   ASSERT(col >= 0);
   ASSERT(row >= 0);
   ASSERT(col < m_nCols);
   ASSERT(row < m_nRows);
   // use existing cell object (if there is one) or a new one
   Cell cell;
   pair<int,int> coord = make_pair(col,row);
   map<pair<int,int>,Cell>::iterator i = m_cells.find(coord);
   if (i != m_cells.end())
   {
      cell = i->second;
      if (cell.m_marked2 == mark) return;	// abort if no change
   }
   // set cell mark
	if(!cell.m_disabled) 
	{
		cell.m_marked2 = mark;
		m_cells[coord] = cell;	// store the cell object back in the map
	}
   // update
   //Invalidate(FALSE);
   //CalculateRowHeights();
   //UpdateChildControls();
}

CString CMultilineList::GetCellText(int col, int row)
{
   ASSERT(col >= 0);
   ASSERT(row >= 0);
   ASSERT(col < m_nCols);
   ASSERT(row < m_nRows);

   CString result;

   // use existing cell object (if there is one) or use default
   Cell cell;
   map<pair<int,int>,Cell>::iterator i = m_cells.find(make_pair(col,row));
   if (i != m_cells.end())
   {
      cell = i->second;
   }

   // return the cell text
   result = cell.m_text;

   return result;
}

void CMultilineList::SetSelRow(int row)
{
   ASSERT(row >= 0);
   ASSERT(row < m_nRows);

   // force selected row to the specified value
   m_curSelRow = row;

   // update
   Invalidate(FALSE);
   UpdateChildControls();
}

int CMultilineList::GetSelRow()
{
   // return currently selected row
   return m_curSelRow;
}

int CMultilineList::GetRowFromPoint(CPoint pt)
{
   int result = -1;

   CalculateRowHeights();

   // ensure point is inside content area
   CRect cr;
   GetContentRect(cr);
   if (cr.PtInRect(pt))
   {
      // convert to list coords
      int listYPos = m_viewYPos + (pt.y - cr.top);

      // walk through the rows until we find one at the specified vertical location
      int row = 0;
      for (int yPos = 0; row < m_nRows; row++)
      {
         int thisRowHeight = m_rowHeights[row];

         if ((yPos + thisRowHeight) > listYPos)
         {
            break;
         }
         else
         {
            yPos += thisRowHeight;
         }
      }

      // did we find a row?
      if (row < m_nRows)
      {
         result = row;

		// //find col-cafvt custom
		//for (int col = 0; col < m_nCols; col++)
		//{
		//	Column column;
		//	std::map<int,Column>::iterator i = m_columns.find(col);
		//	if (i != m_columns.end()) column = i->second;
		//	if (pt.x > column.m_width)
		//		return result;
		//}

      }
   }

   return result;
}

void CMultilineList::EnsureRowIsVisible(int row)
{
   ASSERT(row >= 0);
   ASSERT(row < m_nRows);

   // ensure row heights are up to date
   CalculateRowHeights();

   // find top coord of specified row
   int rowYPos = 0;
   for (int _row = 0; _row < m_nRows; _row++)
   {
      if (_row == row)
      {
         break;
      }
      else
      {
         rowYPos += m_rowHeights[_row];
      }
   }

   // height of specified row
   int rowHeight = m_rowHeights[row];

   // content rect
   CRect cr;
   GetContentRect(cr);

   // top of row is above visible area
   if (rowYPos < m_viewYPos)
   {
      // scroll up so that top of view is the top of the row
      m_viewYPos = rowYPos;

      // update
      Invalidate(FALSE);
      UpdateChildControls();
   }
   // bottom of row is below visible area
   else if ((rowYPos + rowHeight) > (m_viewYPos + cr.Height()))
   {
      // assuming row is smaller than the viewable area,
      // scroll down so that bottom of view includes the bottom of the row
      if (rowHeight < cr.Height())
      {
         m_viewYPos += ((rowYPos + rowHeight) - (m_viewYPos + cr.Height()));
      }
      else
      {
         // if row is taller than the viewable area, then just scroll so
         // the top of the view is at the top of the row
         m_viewYPos = rowYPos;
      }

      // update
      Invalidate(FALSE);
      UpdateChildControls();
   }
}

////////////////////////////////////////////////////////////////////////////////
//                                  PRIVATE
////////////////////////////////////////////////////////////////////////////////

BOOL CMultilineList::RegisterWindowClass()
{
   HINSTANCE hInst = AfxGetInstanceHandle();

   WNDCLASS wndcls;
   ZeroMemory(&wndcls,sizeof(WNDCLASS));

   if (!(::GetClassInfo(hInst, MULTILINELIST_CLASSNAME, &wndcls)))
   {
      wndcls.style            = CS_DBLCLKS;
      wndcls.lpfnWndProc      = ::DefWindowProc;
      wndcls.cbClsExtra       = 0;
      wndcls.cbWndExtra       = 0;
      wndcls.hInstance        = hInst;
      wndcls.hIcon            = NULL;
      wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
      wndcls.lpszMenuName     = NULL;
      wndcls.lpszClassName    = MULTILINELIST_CLASSNAME;

      if (!AfxRegisterClass(&wndcls))
      {
         AfxThrowResourceException();
         return FALSE;
      }
   }

   return TRUE;
}

void CMultilineList::PreSubclassWindow()
{
	// add a border
	ModifyStyle(GetSafeHwnd(),0,WS_BORDER,0);

	// KLUDGE - after adding the border above
	// this seems to be necessary to get the border
	// to initially be drawn
	CRect wr;
	GetWindowRect(wr);
	GetParent()->ScreenToClient(wr);
	wr.right -= 1;
	MoveWindow(wr);
	wr.right += 1;
	MoveWindow(wr);
	/////////////////////

   PrepareOffscreenSurface();

   // create GDI objects used in rendering
   m_gridPen.CreatePen(PS_SOLID,GRID_WIDTH,RGB(200,220,220));//GetSysColor(COLOR_WINDOWTEXT));
   //m_gridPen.CreatePen(PS_SOLID,GRID_WIDTH,GetSysColor(COLOR_WINDOWTEXT));

   // create default font matching parent's font
   LOGFONT lf;
   GetParent()->GetFont()->GetLogFont(&lf);
   //memset(&lf, 0x00, sizeof(lf));
   //memcpy(lf.lfFaceName, TEXT("Microsoft Sans Serif"), 24);

   //lf.lfWeight = FW_BOLD;
   m_defaultFont.CreateFontIndirect(&lf);
   m_headerFont.CreateFontIndirect(&lf);

   // initialize child controls
   CreateChildControls();
   PositionChildControls();

   CWnd::PreSubclassWindow();
}

void CMultilineList::OnSize(UINT nType, int cx, int cy)
{
   CWnd::OnSize(nType, cx, cy);

   PositionChildControls();
   UpdateChildControls();
}

BOOL CMultilineList::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   NMHDR * pNMHDR = (NMHDR*)lParam;

   // header control notifications
   if (wParam == CHILD_ID_HEADERCTRL)
   {
      NMHEADER * pNMHEADER = (NMHEADER*)pNMHDR;

      // user is resizing a column
      if (pNMHDR->code == HDN_TRACK)
      {
         // use existing column object (if there is one) or use default
         Column column;
         std::map<int,Column>::iterator i = m_columns.find(pNMHEADER->iItem);
         if (i != m_columns.end())
         {
            column = i->second;
         }

         // update the width of the column object and store it back in the map
         column.m_width = pNMHEADER->pitem->cxy;
         m_columns[pNMHEADER->iItem] = column;

         // update
         Invalidate(FALSE);
         UpdateChildControls();
      }

      // user has finished resizing a column
      if (pNMHDR->code == HDN_ENDTRACK)
      {
         // use existing column object (if there is one) or use default
         Column column;
         std::map<int,Column>::iterator i = m_columns.find(pNMHEADER->iItem);
         if (i != m_columns.end())
         {
            column = i->second;
         }

         // update the width of the column object and store it back in the map
         column.m_width = pNMHEADER->pitem->cxy;
         m_columns[pNMHEADER->iItem] = column;

         // invalidate all row heights
         m_rowHeights.clear();

         // update
         Invalidate(FALSE);
         CalculateRowHeights();
         UpdateChildControls();
      }
   }

   return CWnd::OnNotify(wParam, lParam, pResult);
}

void CMultilineList::CreateChildControls()
{
   m_headerCtrl.Create(
      CCS_TOP|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),
      this,CHILD_ID_HEADERCTRL);
   m_headerCtrl.SetFont(&m_headerFont);

   m_scrollBar.Create(
      SBS_RIGHTALIGN|SBS_VERT|WS_CHILD|WS_VISIBLE,
      CRect(0,0,0,0),this,CHILD_ID_SCROLLBAR);

   m_scrollBarHorz.Create(
      SBS_BOTTOMALIGN|SBS_HORZ|WS_CHILD|WS_VISIBLE,
      CRect(0,0,0,0),this,CHILD_ID_SCROLLBARHORZ);
}

void CMultilineList::PositionChildControls()
{
   // ensure the child controls exist
   if ((m_headerCtrl.GetSafeHwnd() == NULL) ||
       (m_scrollBar.GetSafeHwnd() == NULL) ||
       (m_scrollBarHorz.GetSafeHwnd() == NULL))
       return;

   CRect cr;
   GetClientRect(cr);

   // get text height
   CDC * pDC = GetDC();
   CFont * origFont = pDC->SelectObject(&m_headerFont);
   TEXTMETRIC tm;
   pDC->GetOutputTextMetrics(&tm);
   pDC->SelectObject(origFont);
   ReleaseDC(pDC);

   // header control
   CRect hdrRect = cr;
   hdrRect.bottom = hdrRect.top + tm.tmHeight + HEADERCTRL_HEIGHT_EXTRA;
   m_headerCtrl.MoveWindow(hdrRect);

   // scrollbar
   CRect sbRect = cr;
   sbRect.left = sbRect.right - GetSystemMetrics(SM_CXVSCROLL);
   sbRect.top = sbRect.top + GetSystemMetrics(SM_CYHSCROLL);
   m_scrollBar.MoveWindow(sbRect);

   // horiz scrollbar
   CRect sbRectHorz = cr;
   sbRectHorz.top = sbRectHorz.bottom - GetSystemMetrics(SM_CYHSCROLL);
   if (m_scrollBar.IsWindowVisible())
   {
      sbRectHorz.right = sbRectHorz.right - GetSystemMetrics(SM_CXVSCROLL);
   }
   m_scrollBarHorz.MoveWindow(sbRectHorz);

   // adjust left edge of the header control to be at the left edge of the list
   // this causes the header control to scroll horizontally with the content
   if (m_viewXPos > 0)
   {
      CRect contentRect;
      GetContentRect(contentRect);

      CRect hr;
      m_headerCtrl.GetWindowRect(hr);
      ScreenToClient(hr);
      hr.left = contentRect.left - m_viewXPos;
      m_headerCtrl.MoveWindow(hr);
   }
}

void CMultilineList::UpdateChildControls()
{
   // get content rect
   CRect contentRect;
   GetContentRect(contentRect);

   // remove excess header control items
   while (m_headerCtrl.GetItemCount() > m_nCols)
   {
      if (!m_headerCtrl.DeleteItem(m_headerCtrl.GetItemCount()-1))
         break;
   }

   // add header control items if needed
   while (m_headerCtrl.GetItemCount() < m_nCols)
   {
      HDITEM hdi;
      ZeroMemory(&hdi,sizeof(HDITEM));
      hdi.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
      hdi.cxy = DEFAULT_COLUMN_WIDTH;
      hdi.fmt = HDF_STRING | HDF_CENTER;      
      hdi.pszText = _T("");
      hdi.cchTextMax = 0;
	  

      if (m_headerCtrl.InsertItem(m_headerCtrl.GetItemCount(),&hdi) < 0)
         break;
   }

   // adjust left edge of the header control to be at the left edge of the list
   // this makes the header control scroll horizontally with the list content
   CRect hr;
   m_headerCtrl.GetWindowRect(hr);
   ScreenToClient(hr);
   hr.left = contentRect.left - m_viewXPos;
   m_headerCtrl.MoveWindow(hr);

   // for each header control item
   for (int col = 0; col < m_nCols; col++)
   {
      // use existing object (if there is one) or default
      Column column;
      map<int,Column>::iterator i = m_columns.find(col);
      if (i != m_columns.end())
      {
         column = i->second;
      }

      HDITEM hdi;
      ZeroMemory(&hdi,sizeof(HDITEM));
      hdi.mask = HDI_TEXT | HDI_WIDTH;

      // get current item attributes
      if (m_headerCtrl.GetItem(col,&hdi))
      {
         // if different, update header control item
         CString curHeading(hdi.pszText);
         if ((curHeading != column.m_heading) ||
               (hdi.cxy != column.m_width))
         {
            hdi.pszText = (LPTSTR)((LPCTSTR)(column.m_heading));
            hdi.cchTextMax = column.m_heading.GetLength();
            hdi.cxy = column.m_width;

            m_headerCtrl.SetItem(col, &hdi);
         }
      }
   }

   // compute height of the entire list
   int totalHeight = 0;
   map<int,int>::iterator iRowHeights;
   for (iRowHeights = m_rowHeights.begin(); iRowHeights != m_rowHeights.end(); ++iRowHeights)
   {
      totalHeight += iRowHeights->second;
   }

   // should the vert scrollbar be enabled?
   if (totalHeight <= contentRect.Height())
   {
      // no, ensure its disabled
      if (m_scrollBar.IsWindowEnabled())
      {
         m_scrollBar.EnableWindow(FALSE);
         m_scrollBar.ShowWindow(SW_HIDE);
      }
   }
   else
   {
      // ensure its enabled
      if (!m_scrollBar.IsWindowEnabled())
      {
         m_scrollBar.EnableWindow(TRUE);
         m_scrollBar.ShowWindow(SW_SHOW);
      }

      // correct values for scrollbar
      int nMin = 0;
      int nMax = totalHeight;
      int nPage = contentRect.Height();
      int nPos = m_viewYPos;

      // get current scrollbar values
      SCROLLINFO si;
      ZeroMemory(&si,sizeof(SCROLLINFO));
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask = SIF_ALL;
      m_scrollBar.GetScrollInfo(&si);

      // if any of the values are different, update scrollbar
      if ((nMin != si.nMin) ||
          (nMax != si.nMax) ||
          (nPage != si.nPage) ||
          (nPos != si.nPos))
      {
         si.nMin = nMin;
         si.nMax = nMax;
         si.nPage = nPage;
         si.nPos = nPos;

         m_scrollBar.SetScrollInfo(&si);
      }
   }

   // compute total width of the list
   int totalWidth = 0;
   for (int col = 0; col < m_nCols; col++)
   {
      // use existing object (if there is one) or default
      Column column;
      map<int,Column>::iterator iColumn = m_columns.find(col);
      if (iColumn != m_columns.end())
      {
         column = iColumn->second;
      }
      
      totalWidth += column.m_width;
   }

   // should horz scrollbar be enabled?
   if (totalWidth <= contentRect.Width())
   {
      // ensure its disabled and hidden
      if (m_scrollBarHorz.IsWindowEnabled())
      {
         m_scrollBarHorz.EnableWindow(FALSE);
         m_scrollBarHorz.ShowWindow(SW_HIDE);
      }
   }
   else
   {
      // ensure its enabled and visible
      if (!m_scrollBarHorz.IsWindowEnabled())
      {
         m_scrollBarHorz.EnableWindow(TRUE);
         m_scrollBarHorz.ShowWindow(SW_SHOW);
      }

      // correct values for scrollbar
      int nMin = 0;
      int nMax = totalWidth;
      int nPage = contentRect.Width();
      int nPos = m_viewXPos;

      // get current scrollbar values
      SCROLLINFO si;
      ZeroMemory(&si,sizeof(SCROLLINFO));
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask = SIF_ALL;
      m_scrollBarHorz.GetScrollInfo(&si);

      // if any of the values are different, update scrollbar
      if ((nMin != si.nMin) ||
          (nMax != si.nMax) ||
          (nPage != si.nPage) ||
          (nPos != si.nPos))
      {
         si.nMin = nMin;
         si.nMax = nMax;
         si.nPage = nPage;
         si.nPos = nPos;

         m_scrollBarHorz.SetScrollInfo(&si);
      }
   }
}

void CMultilineList::CalculateRowHeights()
{
   CFont * origFont = m_offscreenDC.SelectObject(&m_defaultFont);
   CBitmap * origBitmap = m_offscreenDC.SelectObject(&m_offscreenBitmap);

   // for all rows
   for (int row = 0; row < m_nRows; row++)
   {
      // if a row height is not already available for this row
      map<int,int>::iterator iRow = m_rowHeights.find(row);
      if (iRow == m_rowHeights.end())
      {
         int rowHeight = 0;

         // compute heights of each cell, finding the tallest one
         for (int col = 0; col < m_nCols; col++)
         {
            // get column object (if one exists) or use the default
            Column column;
            std::map<int,Column>::iterator iCol = m_columns.find(col);
            if (iCol != m_columns.end())
            {
               column = iCol->second;
            }

            // get cell object (if one exists) or use the default
            Cell cell;
            map<pair<int,int>,Cell>::iterator iCell = m_cells.find(make_pair(col,row));
            if (iCell != m_cells.end())
            {
               cell = iCell->second;
            }

            CRect textRect(0,0,column.m_width,0);
            textRect.DeflateRect(INNER_PADDING,INNER_PADDING);
            int origRight = textRect.right;

            // compute cell height
            m_offscreenDC.DrawTextEx(cell.m_text,textRect,
               DT_CALCRECT|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK,
               NULL);

            textRect.right = origRight;
            textRect.InflateRect(INNER_PADDING,INNER_PADDING);
            textRect.bottom += GRID_WIDTH;

            if (textRect.bottom > rowHeight)
            {
               rowHeight = textRect.bottom;
            }
         }

         // store row height
         m_rowHeights[row] = rowHeight;
      }
   }

   m_offscreenDC.SelectObject(origBitmap);
   m_offscreenDC.SelectObject(origFont);
}


void CMultilineList::GetContentRect(CRect & r)
{
   // get geom of client area and child controls
   CRect cr;
   GetClientRect(cr);
   CRect hdrRect;
   m_headerCtrl.GetClientRect(hdrRect);
   CRect sbRect;
   m_scrollBar.GetClientRect(sbRect);
   CRect sbRectHorz;
   m_scrollBarHorz.GetClientRect(sbRectHorz);

   // shrink for child controls
   r = cr;
   r.top += hdrRect.Height();
   r.right -= sbRect.Width();
   r.bottom -= sbRectHorz.Height();

   // vert scroll bar not visible; extend area to cover where it would be
   if (!m_scrollBar.IsWindowVisible())
   {
      r.right += sbRect.Width();
   }

   // horz scroll bar not visible; extend area to cover where it would be
   if (!m_scrollBarHorz.IsWindowVisible())
   {
      r.bottom += sbRectHorz.Height();
   }
}

BOOL CMultilineList::OnEraseBkgnd(CDC* pDC)
{
   // do nothing; we don't want to erase the background because we
   // use double-buffering and so overwrite the entire client area
   // with every repaint
   return TRUE;
}

void CMultilineList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

   CalculateRowHeights();

   // get content rect
   CRect cr;
   GetContentRect(cr);

   if ((nSBCode == SB_THUMBPOSITION) ||
       (nSBCode == SB_THUMBTRACK))
   {
      // set pos to specified absolute pos
      m_viewYPos = nPos;
   }

   else if (nSBCode == SB_PAGEUP)
   {
      // set pos up by 1 page
      m_viewYPos -= cr.Height();
   }

   else if (nSBCode == SB_PAGEDOWN)
   {  
      // compute height of the entire list
      int totalHeight = 0;
      map<int,int>::iterator iRowHeights;
      for (iRowHeights = m_rowHeights.begin(); iRowHeights != m_rowHeights.end(); ++iRowHeights)
      {
         totalHeight += iRowHeights->second;
      }

      // move down by 1 page or to end if there's less
      // than one page remaining outside the view
      int remainingSpace = totalHeight - (m_viewYPos + cr.Height());
      if (remainingSpace < cr.Height())
      {
         m_viewYPos += remainingSpace;
      }
      else
      {
         m_viewYPos += cr.Height();
      }
   }

   else if ((nSBCode == SB_LINEUP) || (nSBCode == SB_LINEDOWN))
   {
      // determine top visible row
      int y1 = 0;
      int row = 0;
      for (int yPos = 0; row < m_nRows; row++)
      {
         int thisRowHeight = m_rowHeights[row];

         if ((yPos + thisRowHeight) > m_viewYPos)
         {
            break;
         }
         else
         {
            yPos += thisRowHeight;
         }
      }

      // determine remaining height in list (including and after the top visible row)
      int remainingHeight = 0;
      for (int row2 = row; row2 < m_nRows; row2++)
      {
         remainingHeight += m_rowHeights[row2];
      }

      // for scrolling up, use one row above the top visible row
      if (nSBCode == SB_LINEUP)
      {
         row--;
         if (row < 0)
            row = 0;
      }

      // move by the height of the top row
      int deltaY = m_rowHeights[row];
      if (nSBCode == SB_LINEUP)
      {
         deltaY *= -1;
      }

      // assuming there's more to scroll (or if we're scrolling up)
      if (remainingHeight > cr.Height() || (nSBCode == SB_LINEUP))
      {
         m_viewYPos += deltaY;

         if (m_viewYPos < 0)
         {
            m_viewYPos = 0;
         }
      }
   }

   // limit upward scrolling
   if (m_viewYPos < 0)
   {
      m_viewYPos = 0;
   }

   // update
   Invalidate(TRUE);
   UpdateChildControls();
}

void CMultilineList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   CWnd::OnHScroll(nSBCode, nPos, pScrollBar);

   // compute total width of the list
   int totalWidth = 0;
   for (int col = 0; col < m_nCols; col++)
   {
      // use existing object (if there is one) or default
      Column column;
      map<int,Column>::iterator iColumn = m_columns.find(col);
      if (iColumn != m_columns.end())
      {
         column = iColumn->second;
      }
      
      totalWidth += column.m_width;
   }

   if ((nSBCode == SB_THUMBPOSITION) ||
       (nSBCode == SB_THUMBTRACK))
   {
      // set pos to specified absolute pos
      m_viewXPos = nPos;
   }

   else if (nSBCode == SB_PAGELEFT)
   {
      CRect cr;
      GetContentRect(cr);

      // set pos up by left 1 page
      m_viewXPos -= cr.Width();
   }

   else if (nSBCode == SB_PAGERIGHT)
   {
      CRect cr;
      GetContentRect(cr);
      
      // move right by 1 page or to end if there's less
      // than one page remaining outside the view
      int remainingSpace = totalWidth - (m_viewXPos + cr.Width());
      if (remainingSpace < cr.Width())
      {
         m_viewXPos += remainingSpace;
      }
      else
      {
         m_viewXPos += cr.Width();
      }
   }

   else if (nSBCode == SB_LINELEFT)
   {      
      // set pos left by constant amount
      m_viewXPos -= HORZSCROLL_PIXELS;
   }

   else if (nSBCode == SB_LINERIGHT)
   {
      CRect cr;
      GetContentRect(cr);

      // don't scroll right unless there's more to scroll to
      if ((totalWidth - m_viewXPos) > cr.Width())
      {
         // set pos right by constant amount
         m_viewXPos += HORZSCROLL_PIXELS;
      }
   }

   // limit scrolling
   if (m_viewXPos < 0)
   {
      m_viewXPos = 0;
   }

   Invalidate(TRUE);
   UpdateChildControls();
}

BOOL CMultilineList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
   // scroll up/down 1 line when mouse-wheel is used
   if (zDelta < 0)
   {
      OnVScroll(SB_LINEDOWN,0,NULL);
   }
   else
   {
      OnVScroll(SB_LINEUP,0,NULL);
   }

   return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CMultilineList::OnLButtonDown(UINT nFlags, CPoint point)
{
	//cafvt custom
	m_markXPos = point.x;
	m_markYPos = point.y;
	//reset view pos if scrolled
   //m_viewXPos = 0;
   //m_viewYPos = 0;

   // on click, select the row which was clicked on
	m_curSelRow = GetRowFromPoint(point);
   Invalidate(FALSE);
   UpdateChildControls();

 
   // ensure the selected row is visible
   if (m_curSelRow >= 0)
   {
      EnsureRowIsVisible(m_curSelRow);
   }

	ClientToScreen(&point);
	m_XPos = point.x;
	m_YPos = point.y;

   // sent notification message to parent window
   int thisCtrlID = GetDlgCtrlID();
   GetParent()->PostMessage(WM_COMMAND,MAKEWPARAM(thisCtrlID,LBN_SELCHANGE),(LPARAM)GetSafeHwnd());
}

void CMultilineList::OnPaint()
{
   CPaintDC dc(this);

   // ensure offscreen rendering surface is ready
   if (!PrepareOffscreenSurface())
      return;

   // ensure GDI objects are ready and match current system preferences

   //LOGPEN lp;
   //m_gridPen.GetLogPen(&lp);
   //if (lp.lopnColor != GetSysColor(COLOR_WINDOWTEXT))
   //{
   //   m_gridPen.DeleteObject();
   //   m_gridPen.CreatePen(PS_SOLID,GRID_WIDTH,GetSysColor(COLOR_WINDOWTEXT));
   //}

   // ensure row height calculations are up to date
   CalculateRowHeights();

   // get content area (client area minus child controls)
   CRect cr;
   GetClientRect(cr);

   CFont * origFont = m_offscreenDC.SelectObject(&m_defaultFont);
   CBitmap * origBitmap = m_offscreenDC.SelectObject(&m_offscreenBitmap);

   // render the content area
   CRect renderRect;
   GetContentRect(renderRect);
   RenderContent(m_offscreenDC,renderRect);

   // blt the rendered content from the offscreen area to the window
   dc.BitBlt(renderRect.left,renderRect.top,
      renderRect.Width(),renderRect.Height(),
      &m_offscreenDC,
      renderRect.left,renderRect.top,
      SRCCOPY);

   m_offscreenDC.SelectObject(origBitmap);
   m_offscreenDC.SelectObject(origFont);
}

BOOL CMultilineList::PrepareOffscreenSurface()
{
   CRect wr;
   GetWindowRect(wr);
   CSize ws = wr.Size();

   // not yet created or the size has changed
   if ((m_offscreenBitmap.GetSafeHandle() == NULL) ||
       (m_offscreenBitmapSize != ws))
   {
      CDC * pDC = GetDC();

      // create offscreen DC if necessary
      if (m_offscreenDC.GetSafeHdc() == NULL)
      {
         if (!m_offscreenDC.CreateCompatibleDC(pDC))
            return FALSE;
      }

      // delete existing bitmap (if there is one)
      if (m_offscreenBitmap.GetSafeHandle() != NULL)
      {
         m_offscreenBitmap.DeleteObject();
      }

      // create new bitmap sized to the window
      if (!m_offscreenBitmap.CreateCompatibleBitmap(pDC,ws.cx,ws.cy))
         return FALSE;

      // store the new size
      m_offscreenBitmapSize = ws;

      ReleaseDC(pDC);
   }

   return TRUE;
}

void CMultilineList::RenderContent(CDC & dc, CRect & r)
{
   dc.FillRect(r,CBrush::FromHandle(GetSysColorBrush(COLOR_WINDOW)));

   dc.SetBkMode(TRANSPARENT);
   dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

   CPen * origPen = dc.SelectObject(&m_gridPen);

   int y1 = 0;
   int row = 0;

   // determine top visible row and setup y1 to be the top of its rendering area
   for (int yPos = 0; row < m_nRows; row++)
   {
      int thisRowHeight = m_rowHeights[row];

      if ((yPos + thisRowHeight) > m_viewYPos)
      {
         y1 = r.top - (m_viewYPos - yPos);
         break;
      }
      else
      {
         yPos += thisRowHeight;
      }
   }

   int firstRenderedRow = row;

	// for each row
   for (; (row < m_nRows) && (y1 <= r.bottom); row++)
   {
		// start with zero height
      int y2 = y1;

		// for each column
	  int x1 = r.left-m_viewXPos;
      for (int col = 0/*, x1 = r.left-m_viewXPos*/; (col < m_nCols); col++)
      {
			// get the column object (if it exists) or use defaults
         Column column;
         std::map<int,Column>::iterator i = m_columns.find(col);
         if (i != m_columns.end())
         {
            column = i->second;
         }

			// get the cell object (if it exists) or use defaults
         Cell cell;
		pair<int,int> coord = make_pair(col,row);
        map<pair<int,int>,Cell>::iterator j = m_cells.find(make_pair(col,row));
         if (j != m_cells.end())
         {
            cell = j->second;
         }

			// determine the required size of the text, given the column width

         int x2 = x1 + column.m_width;

         CRect textRect(x1,y1,x2,y1);
         textRect.DeflateRect(INNER_PADDING,INNER_PADDING);
         int origRight = textRect.right;
         
         dc.DrawTextEx(cell.m_text,textRect,
            DT_CALCRECT|DT_LEFT|DT_NOPREFIX|DT_TOP|DT_WORDBREAK,
            NULL);
         
         textRect.right = origRight;
		 //textRect.top += 2;

			// if this row is selected, fill in the background with the selection color and
			// set the text color
         //if (row == m_curSelRow)//cafvt custom
         //if (row == m_nHighlightedRow)
         //{
         //   CRect bgRect(textRect);
         //   bgRect.InflateRect(INNER_PADDING,INNER_PADDING);
         //   bgRect.bottom = bgRect.top + m_rowHeights[row] + GRID_WIDTH;
         //   bgRect.top += GRID_WIDTH;
         //   dc.FillRect(bgRect,CBrush::FromHandle(GetSysColorBrush(COLOR_INFOBK))); //cafvt custom
         //   //dc.FillRect(bgRect,CBrush::FromHandle(GetSysColorBrush(COLOR_HIGHLIGHT)));
         //}
        if (cell.m_disabled)
         {
            dc.SetTextColor(RGB(200,200,200));
		 }
          else
         {
			// else, ensure text color is set to the non-selected color
            //dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
            dc.SetTextColor(cell.m_TextColor);
         }

         textRect.bottom += INNER_PADDING;
         textRect.bottom += GRID_WIDTH;

			// ensure tallest cell is stored
         if (textRect.bottom > y2)
         {
            y2 = textRect.bottom;
         }

		 //store cell rect
		 cell.cx1=textRect.left;
		 cell.cx2=textRect.right;
		 cell.cy1=textRect.top;
		 cell.cy2=textRect.bottom;
		 m_cells[coord] = cell;


		 //mark cell as selected 
		 //if(m_IsMarkable)
		 //{
			//if(m_markXPos>textRect.left  && m_markXPos<textRect.right)
			//{
			//	if(m_markYPos>textRect.top  && m_markYPos<textRect.bottom)
			//	{
			//		m_selcol = coord.first;
			//		m_selrow = coord.second;
			//		if(m_selrow>0)
			//		{
			//			cell.m_marked = !cell.m_marked;
			//			m_markXPos = 0;
			//			m_markYPos = 0;
			//			// store the cell object back in the map
			//			m_cells[coord] = cell;
			//			m_sel = cell.m_marked;
			//		}
			//	}
			//}

			if(cell.m_marked2) 
			{
				CBrush markbrush(RGB(240,240,250));
				CRect markrect(textRect);
				markrect.bottom +=2;
				markrect.left -=3;
				dc.FillRect(markrect,&markbrush);
			}
			if(cell.m_marked) 
			{
				CBrush markbrush(RGB(255,220,150));
				CRect markrect(textRect);
				markrect.bottom +=2;
				markrect.left -=3;
				markrect.DeflateRect(2,2);
				dc.FillRect(markrect,&markbrush);
			}
			//}

			// render the cell text
         dc.DrawTextEx(cell.m_text,textRect,
            cell.m_align|DT_NOPREFIX|DT_TOP|DT_WORDBREAK,
            NULL);

         x1 = x2;
      }

		// render horizontal grid line below the current row
      //dc.MoveTo(r.left,y2+GRID_WIDTH+50); //cafvt customization
      //dc.LineTo(x1,y2+GRID_WIDTH+50);
      dc.MoveTo(r.left,y2+GRID_WIDTH);
      dc.LineTo(x1,y2+GRID_WIDTH);

		// if not the first row, also render the top horizontal line above the current row
      if (row > firstRenderedRow)
      {
         dc.MoveTo(r.left,y1+GRID_WIDTH);
         dc.LineTo(x1,y1+GRID_WIDTH);
      }

		// render the vertical lines between the columns
      //for (int col = 0, x1 = r.left-m_viewXPos; (col < m_nCols) && (x1 <= r.right); col++)
      for (int col = 0, x1 = r.left-m_viewXPos; (col < m_nCols-1) && (x1 <= r.right); col++)//cafvt custom
      {
         Column column;
         std::map<int,Column>::iterator i = m_columns.find(col);
         if (i != m_columns.end())
         {
            column = i->second;
         }

         int x2 = x1 + column.m_width;

         dc.MoveTo(x2-2,y1+GRID_WIDTH); //naad customization
         dc.LineTo(x2-2,y2+GRID_WIDTH+50);
         //dc.MoveTo(x2,y1+GRID_WIDTH);
         //dc.LineTo(x2,y2+GRID_WIDTH);


         x1 = x2;
      }

      y1 = y2;
   }

   dc.SelectObject(origPen);
}

void CMultilineList::GetCellSelection()
{
	m_selcol = 0;
	m_selrow = 0;
	 for (int col = 0; col<m_nCols; col++)
      {
	 for (int row = 0; row<m_nRows; row++)
      {
		// get the cell object (if it exists) or use defaults
        Cell cell;
		pair<int,int> coord = make_pair(col,row);
        map<pair<int,int>,Cell>::iterator j = m_cells.find(make_pair(col,row));
        if (j != m_cells.end()) cell = j->second;
		 //mark cell as selected - cafvt custom
		 if((m_IsMarkable)&&(!cell.m_disabled))
		 {
			if(m_markXPos>cell.cx1-2  && m_markXPos<cell.cx2+2)
			{
				if(m_markYPos>cell.cy1-2  && m_markYPos<cell.cy2+2)
				{
					m_selcol = coord.first;
					m_selrow = coord.second;
					if(m_selrow>=0)
					{
						cell.m_marked = !cell.m_marked;
						m_markXPos = 0;
						m_markYPos = 0;
						// store the cell object back in the map
						m_cells[coord] = cell;
						m_sel = cell.m_marked;
						return;
					}
				}
			  }
			}
		
	  }
	  }

}

BOOL CMultilineList::GetColStatus(int ncol)
{
	int res = 0;
	  for (int row = 0; row<m_nRows; row++)
      {
		// get the cell object (if it exists) or use defaults
        Cell cell;
		pair<int,int> coord = make_pair(ncol,row);
        map<pair<int,int>,Cell>::iterator j = m_cells.find(make_pair(ncol,row));
        if (j != m_cells.end()) cell = j->second;
		
		res += cell.m_marked;
		
	  }
	  if(res > 0) return true;
	  return false;

}

void CMultilineList::SetCellDisable(int col, int row, BOOL dis)
{
	ASSERT(col >= 0);
   ASSERT(row >= 0);
   ASSERT(col < m_nCols);
   ASSERT(row < m_nRows);

   // use existing cell object (if there is one) or a new one
   Cell cell;
   pair<int,int> coord = make_pair(col,row);
   map<pair<int,int>,Cell>::iterator i = m_cells.find(coord);
   if (i != m_cells.end())
   {
      cell = i->second;

      // abort if no change
      if (cell.m_disabled == dis) return;
   }

   // set cell disable
	cell.m_disabled = dis;

   // store the cell object back in the map
   m_cells[coord] = cell;

   Invalidate(false);
}