// SnakeDoc.cpp : implementation of the CSnakeDoc class
//

#include "stdafx.h"
#include "Snake.h"

#include "SnakeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSnakeDoc

IMPLEMENT_DYNCREATE(CSnakeDoc, CDocument)

BEGIN_MESSAGE_MAP(CSnakeDoc, CDocument)
	//{{AFX_MSG_MAP(CSnakeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnakeDoc construction/destruction

CSnakeDoc::CSnakeDoc()
{
	// TODO: add one-time construction code here
	// ��ʼ������
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1,1);
	// Ĭ�ϱ���ɫ����ɫ
	//m_refColorBKG = 0x00808080;
}

CSnakeDoc::~CSnakeDoc()
{
}

BOOL CSnakeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSnakeDoc serialization

void CSnakeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSnakeDoc diagnostics

#ifdef _DEBUG
void CSnakeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSnakeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSnakeDoc commands

BOOL CSnakeDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	CFile file;
	CFileException fe;
	
	// ���ļ�
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		// ʧ��
		ReportSaveLoadException(lpszPathName, &fe,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		
		// ����FALSE
		return FALSE;
	}
	
	DeleteContents();
	
	// ���Ĺ����״
	BeginWaitCursor();
	
	// ���Ե���ReadDIBFile()��ȡͼ��
	TRY
	{
		m_hDIB = ::ReadDIBFile(file);
	}
	CATCH (CFileException, eLoad)
	{
		// ��ȡʧ��
		file.Abort();
		
		// �ָ������״
		EndWaitCursor();
		
		// ����ʧ��
		ReportSaveLoadException(lpszPathName, eLoad,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		
		// ����DIBΪ��
		m_hDIB = NULL;
		
		// ����FALSE
		return FALSE;
	}
	END_CATCH
		
	// ��ʼ��DIB
	InitDIBData();
	
	// �ָ������״
	EndWaitCursor();
	
	// �ж϶�ȡ�ļ��Ƿ�ɹ�
	if (m_hDIB == NULL)
	{
		// ʧ�ܣ����ܷ�BMP��ʽ
		CString strMsg;
		strMsg = "��ȡͼ��ʱ���������ǲ�֧�ָ����͵�ͼ���ļ���";
		
		// ��ʾ����
		MessageBox(NULL, strMsg, "ϵͳ��ʾ", MB_ICONINFORMATION | MB_OK);
		
		// ����FALSE
		return FALSE;
	}
	
	// �����ļ�����
	SetPathName(lpszPathName);
	
	// ��ʼ���ͱ��ΪFALSE
	SetModifiedFlag(FALSE);
	
	// ����TRUE
	return TRUE;
}

void CSnakeDoc::InitDIBData()
{
	// ��ʼ��DIB����
	
	// �жϵ�ɫ���Ƿ�Ϊ��
	if (m_palDIB != NULL)
	{
		// ɾ����ɫ�����
		delete m_palDIB;
		
		// ���õ�ɫ��Ϊ��
		m_palDIB = NULL;
	}
	
	// ���DIB����Ϊ�գ�ֱ�ӷ���
	if (m_hDIB == NULL)
	{
		// ����
		return;
	}
	
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	
	// �ж�ͼ���Ƿ����
	if (::DIBWidth(lpDIB) > INT_MAX ||::DIBHeight(lpDIB) > INT_MAX)
	{
		::GlobalUnlock((HGLOBAL) m_hDIB);
		
		// �ͷ�DIB����
		::GlobalFree((HGLOBAL) m_hDIB);
		
		// ����DIBΪ��
		m_hDIB = NULL;
		
		CString strMsg;
		strMsg = "BMPͼ��̫��";
		
		// ��ʾ�û�
		MessageBox(NULL, strMsg, "ϵͳ��ʾ", MB_ICONINFORMATION | MB_OK);
		
		// ����
		return;
	}
	
	// �����ĵ���С
	m_sizeDoc = CSize((int) ::DIBWidth(lpDIB), (int) ::DIBHeight(lpDIB));
	
	::GlobalUnlock((HGLOBAL) m_hDIB);
	
	// �����µ�ɫ��
	m_palDIB = new CPalette;
	
	// �ж��Ƿ񴴽��ɹ�
	if (m_palDIB == NULL)
	{
		// ʧ�ܣ��������ڴ治��
		::GlobalFree((HGLOBAL) m_hDIB);
		
		// ����DIB����Ϊ��
		m_hDIB = NULL;
		
		// ����
		return;
	}
	
	// ����CreateDIBPalette��������ɫ��
	if (::CreateDIBPalette(m_hDIB, m_palDIB) == NULL)
	{
		// ���ؿգ����ܸ�DIB����û�е�ɫ��
		
		// ɾ��
		delete m_palDIB;
		
		// ����Ϊ��
		m_palDIB = NULL;
		
		// ����
		return;
	}
}