// SnakeDoc.h : interface of the CSnakeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNAKEDOC_H__FCED9539_8EA3_40E5_AF14_B8CFA3898794__INCLUDED_)
#define AFX_SNAKEDOC_H__FCED9539_8EA3_40E5_AF14_B8CFA3898794__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DIB.h"

class CSnakeDoc : public CDocument
{
protected: // create from serialization only
	CSnakeDoc();
	DECLARE_DYNCREATE(CSnakeDoc)

// Attributes
public:
	HDIB m_hDIB;
	CPalette* m_palDIB;
	CSize m_sizeDoc;
	COLORREF m_refColorBKG;// ±³¾°É«

// Operations
public:
	HDIB GetHDIB() const
	{ return m_hDIB; }
	CPalette* GetDocPalette() const
	{ return m_palDIB; }
	CSize GetDocSize() const
	{ return m_sizeDoc; }
	void InitDIBData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSnakeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSnakeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSnakeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNAKEDOC_H__FCED9539_8EA3_40E5_AF14_B8CFA3898794__INCLUDED_)
