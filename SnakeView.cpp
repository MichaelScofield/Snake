// SnakeView.cpp : implementation of the CSnakeView class
//

#include "stdafx.h"
#include "Snake.h"

#include "SnakeDoc.h"
#include "SnakeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSnakeView

IMPLEMENT_DYNCREATE(CSnakeView, CScrollView)

BEGIN_MESSAGE_MAP(CSnakeView, CScrollView)
	//{{AFX_MSG_MAP(CSnakeView)
	ON_COMMAND(ID_DRAW_CONTOUR, OnDrawContour)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_START, OnStart)
	ON_WM_TIMER()
	ON_COMMAND(ID_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnakeView construction/destruction

CSnakeView::CSnakeView()
{
	// TODO: astart construction code here
	start=false;
}

CSnakeView::~CSnakeView()
{
}

BOOL CSnakeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSnakeView drawing

void CSnakeView::OnDraw(CDC* pDC)
{
	CSnakeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	// 获取DIB
	HDIB hDIB = pDoc->GetHDIB();
	
	// 判断DIB是否为空
	if (hDIB != NULL)
	{
		LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
		
		// 获取DIB宽度
		int cxDIB = (int) ::DIBWidth(lpDIB);
		
		// 获取DIB高度
		int cyDIB = (int) ::DIBHeight(lpDIB);
		
		::GlobalUnlock((HGLOBAL) hDIB);
		
		CRect rcDIB;
		rcDIB.top = 0;
		rcDIB.left = 0;
		rcDIB.right = cxDIB;
		rcDIB.bottom = cyDIB;
		
		// 输出DIB
		::PaintDIB(pDC->m_hDC, &rcDIB, pDoc->GetHDIB(),
			&rcDIB, pDoc->GetDocPalette());
	}
}

void CSnakeView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

/////////////////////////////////////////////////////////////////////////////
// CSnakeView diagnostics

#ifdef _DEBUG
void CSnakeView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSnakeView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CSnakeDoc* CSnakeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSnakeDoc)));
	return (CSnakeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSnakeView message handlers

void CSnakeView::OnDrawContour() 
{
	CSnakeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	HDIB hDIB=pDoc->m_hDIB;
	CDC*pDC=GetDC();
	m_pSnake=new SnakeClass(hDIB);
	start=true;
}

void CSnakeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(start)
	{
		CDC*pDC=GetDC();
		m_pSnake->LButtonDown(point,pDC);
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CSnakeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Astart your message handler code here and/or call default
	
	CScrollView::OnRButtonDown(nFlags, point);
}

void CSnakeView::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if(start)
	{
		CDC*pDC=GetDC();
		m_pSnake->LButtonUp(point,pDC);	
	}
	CScrollView::OnLButtonUp(nFlags, point);
}

void CSnakeView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(start)
	{
		CDC*pDC=GetDC();
		m_pSnake->MouseDownAndMove(point,pDC);
	}
	CScrollView::OnMouseMove(nFlags, point);
}

void CSnakeView::OnStart() 
{
	SetTimer(1,200,NULL);
}

void CSnakeView::OnTimer(UINT nIDEvent) 
{
	m_pSnake->Snake_algorithm();
	
	float x,y;
	CDC *pDC;
	pDC=GetDC();
	RedrawWindow();
	CPen pen1(PS_SOLID,2,(COLORREF)255*100),pen2(PS_SOLID,1,(COLORREF)255*255);
	CPen* pOldPen=pDC->SelectObject(&pen1);	
	int i=0;
	pDC->MoveTo(m_pSnake->Snake_points[0]);
	for(i=0;i<m_pSnake->p_NumPos;i++)
	{
		pDC->LineTo(m_pSnake->Snake_points[i]);
		x=m_pSnake->Snake_points[i].x;
		y=m_pSnake->Snake_points[i].y;
		pOldPen=pDC->SelectObject(&pen2);
		pDC->Ellipse(x-2,y-2,x+2,y+2);
	}
	CScrollView::OnTimer(nIDEvent);
}

void CSnakeView::OnStop() 
{
	KillTimer(1);
}
