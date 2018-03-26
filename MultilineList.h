#pragma once

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

//******************************************************************************
// REVISION HISTORY
//------------------------------------------------------------------------------
// 2006.09.04 - initial release
//******************************************************************************

#include <map>
#include <utility>

// default column width in pixels; this is used if width for a column is
// not specified
#define DEFAULT_COLUMN_WIDTH 100
#define TXTBLACK	RGB(0,0,0)
#define TXTRED		RGB(220,0,0)
#define TXTGREEN	RGB(0,160,0)
#define TXTBLUE		RGB(0,0,220)
#define TXTBLACK	RGB(0,0,0)


/**
 * A custom control which provides a multi-column list with varying row heights,
 * allowing multi-line text in the cells.  Cell text is wrapped as necessary and
 * may contain '\n' chars which will force line breaks.
 *
 * To use, set the overall size (# cols, # rows), the column headings, and widths
 * and then set individual cell text.
 *
 * When the current row selection changes, this control will send a WM_COMMAND
 * message to its parent with the LBN_SELCHANGE code.  i.e., it notifies its parent
 * of selection changes using the same technique used by the standard list box.
 *
 * There are 2 ways to use this control in your code.
 *
 * 1) You can create the control dynamically by adding an instance of this class
 *    to your parent window class and then calling Create() with the desired
 *    geometry
 * 2) You can also use the VS dialog editor to allow you to visually place the
 *    control.  To do this, create a custom control, position and size as
 *    desired, and then set the Class property to "CMultilineList" (without the
 *    double-quotes).  You can then right-click on the control and select
 *    "Add Variable" and then use that added variable in your code.
 */
class CMultilineList : public CWnd
{
public:

   CMultilineList();
	virtual ~CMultilineList();

   /**
    * You can use this method to dynamically create an instance of the control.
    */
   BOOL Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle = WS_VISIBLE);

   /**
    * Sets the overall geometry of the grid.  When reducing the size using this
    * method, anything which is then outside the new size is deleted.  You must call
    * this before anything else to ensure the size you need is available.
    * @param nCols Desired number of columns (which are numbered [0...N-1])
    * @param nRows Desired number of rows (which are numbered [0...N-1])
    */
   void SetSize(int nCols, int nRows);

   /**
    * Gets the current overall geometry of the grid.
    * @param nCols will be initialized with the number of columns (which are numbered [0...N-1])
    * @param nRows will be initialized with the number of rows (which are numbered [0...N-1])
    */
   void GetSize(int & nCols, int & nRows);

   /**
    * Sets the text shown in the heading above the specified column.
    * @param col the column number [0...N-1]
    * @param heading the text to be displayed
    */
   void SetColHeading(int col, LPCTSTR heading);

   /**
    * Sets the width of the specified column in pixels.  Note that the user cal
    * also change this themselves by dragging the column separators in the
    * header control.
    * @param col the column number [0...N-1]
    * @param width desired width of the column in pixels
    */
   void SetColWidth(int col, int width);

   /**
    * Sets the text for the specified cell.  The text may contain '\n' which
    * will cause line breaks and heightening of the row as needed.  The text
    * will also be word-wrapped to ensure it fits within the column width.
    * @param col the column number [0...N-1]
    * @param row the row number [0...N-1]
    * @param text the desired text for the cell
    */
   void SetCellText(int col, int row, LPCTSTR text,int align = DT_LEFT, COLORREF color = TXTBLACK);

   /**
    * Gets the current text in the specified cell.
    * @param col the column number [0...N-1]
    * @param row the row number [0...N-1]
    * @return text in the specified cell
    */
   CString GetCellText(int col, int row);

   /**
    * changes the current selection to the specified row.  changing the selection
    * programmatically using this method will not trigger a WM_COMMAND notification
    * message to be sent to the parent window.
    * @param row the row number to select [0...N-1]
    */
   void SetSelRow(int row);

   /**
    * Gets the row number [0...N-1] of the currently selected row.
    * @return the currently selected row number [0...N-1] or -1 if no row is currently selected
    */
   int GetSelRow();

   /**
    * Gets the row which is displayed at the specified client co-ords in the window
    * @param pt the point in client coords
    * @return the row number [0...N-1] which the specified point is over (or -1 if there is no row at that point)
    */
   int GetRowFromPoint(CPoint pt);

   /**
    * Ensure the specified row is visible
    * @param row the row number [0...N-1]
    */
   void EnsureRowIsVisible(int row);

      //naad custom funcs
	void SetCellMark(int col, int row, BOOL mark);
	void SetCellBg(int col, int row, BOOL mark);
	void SetCellDisable(int col, int row, BOOL dis);
	void GetCellSelection(void);
	
	BOOL GetColStatus(int ncol);

	BOOL m_IsMarkable, m_sel;
	int m_nHighlightedRow, m_selrow, m_selcol;
  
	// the overall size of the list (# of columns and # of rows)
   int m_nCols, m_nRows;
   // tracks the position of the top left of the visible area (# pixels relative to top and left of the list)
   int m_viewXPos;
   int m_viewYPos;

   //coord to display pop-up menu //naadh custom
   int m_XPos;
   int m_YPos;


public: // windows message handlers

   /// called to erase the background
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

   /// called to paint the contents of the window
   afx_msg void OnPaint();

   /// called when the window has been resized
   afx_msg void OnSize(UINT nType, int cx, int cy);

   /// called when scrolling occurs using the vertical scrollbar
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

   /// called when scrolling occurs using the horizontal scrollbar
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

   /// called in response to mouse-wheel
   afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

   /// called when user clicks in the window
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:

   /// called at initialization
   virtual void PreSubclassWindow();

   /// called when receiving notification messages from the child controls
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

   /// holds information about a single column
   class Column
   {
	public:

      // column width in pixels
      int m_width;

      // heading text to display above the column in the header control
      CString m_heading;

      Column()
      {
         m_width = DEFAULT_COLUMN_WIDTH;
         m_heading = _T(" ");
      }
   };

   /// holds information about a single cell
   class Cell
   {
   public:

      // text to display in the cell (can contain '\n' which will cause line breaks)
      CString m_text;
	  int m_align;
	  BOOL m_marked, m_marked2;
	  int cx1,cy1,cx2,cy2;
	  BOOL m_disabled;
  	  COLORREF m_TextColor;

      Cell()
      {
         m_text			= _T(" ");
		 m_align		= DT_LEFT;
		 m_marked		= false;
		 m_marked2		= false;
		 m_disabled		= false;
		 m_TextColor	= TXTBLACK;
		 cx1=cx2=cy1=cy2=0;
      }
   };

   // currently selected row
   int m_curSelRow;


   // tracks the position of mouseclick
   int m_markXPos;
   int m_markYPos;


   /// maps column # to Column objects
   std::map<int,Column> m_columns;

   /// maps <col#,row#> to Cell objects
   std::map<std::pair<int,int>,Cell> m_cells;

   /// maps row# to row height in pixels; values are invalidated by removing them; all values in here are valid
   std::map<int,int> m_rowHeights;

   /// registers the window class
   BOOL RegisterWindowClass();

   // the below provide an offscreen rendering surface, allowing us to render
   // to that surface and then blt the resultant image into the window's DC
   CDC     m_offscreenDC;
   CBitmap m_offscreenBitmap;
   CSize   m_offscreenBitmapSize;

   /**
    * child header control, used to display the column headings and to allow
    * dynamic resize of columns by the user
    */
   CHeaderCtrl m_headerCtrl;

   /**
    * child vertical scrollbar control
    */
   CScrollBar  m_scrollBar;

   /**
    * child horizontal scrollbar control
    */
   CScrollBar  m_scrollBarHorz;

   // misc GDI objects used when rendering
   CPen m_gridPen;
   CFont m_defaultFont;
   CFont m_headerFont;

   /// ensures the offscreen drawing surface is available and sized to the window's client area
   BOOL PrepareOffscreenSurface();

   /// creates the child controls during initialization
   void CreateChildControls();

   /// positions/sizes the child controls based on the current window size
   void PositionChildControls();

   /// updates the state of the child controls to match the state of this control
   void UpdateChildControls();

   /// ensures a complete set of valid row heights is available
   void CalculateRowHeights();

   /// calcuates the content area rect in window client coords
   void GetContentRect(CRect & r);

   /// renders the content area (client area minus the border, header control, and scroll bar)
   void RenderContent(CDC & dc, CRect & r);


   DECLARE_DYNAMIC(CMultilineList)
   DECLARE_MESSAGE_MAP()
};


