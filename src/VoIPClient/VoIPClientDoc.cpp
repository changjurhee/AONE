
// VoIPClientDoc.cpp: CVoIPClientDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "VoIPClient.h"
#endif

#include "VoIPClientDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVoIPClientDoc

IMPLEMENT_DYNCREATE(CVoIPClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CVoIPClientDoc, CDocument)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CVoIPClientDoc, CDocument)
END_DISPATCH_MAP()

// 참고: IID_IVoIPClient에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다.
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {ada92bd1-55f0-4922-950a-052e440958fb}
static const IID IID_IVoIPClient =
{0xada92bd1,0x55f0,0x4922,{0x95,0x0a,0x05,0x2e,0x44,0x09,0x58,0xfb}};

BEGIN_INTERFACE_MAP(CVoIPClientDoc, CDocument)
	INTERFACE_PART(CVoIPClientDoc, IID_IVoIPClient, Dispatch)
END_INTERFACE_MAP()


// CVoIPClientDoc 생성/소멸

CVoIPClientDoc::CVoIPClientDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	EnableAutomation();

	AfxOleLockApp();
	m_spUserInfo = std::shared_ptr<userInfo>(new userInfo);

	IsCurrentUser = FALSE;
	m_IsConnected = FALSE;
}

CVoIPClientDoc::~CVoIPClientDoc()
{
	AfxOleUnlockApp();
}

BOOL CVoIPClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}

// CVoIPClientDoc serialization

void CVoIPClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CVoIPClientDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CVoIPClientDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CVoIPClientDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CVoIPClientDoc 진단

#ifdef _DEBUG
void CVoIPClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVoIPClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVoIPClientDoc 명령
