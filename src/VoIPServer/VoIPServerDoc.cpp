
// VoIPServerDoc.cpp: CVoIPServerDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "VoIPServer.h"
#endif

#include "VoIPServerDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVoIPServerDoc

IMPLEMENT_DYNCREATE(CVoIPServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CVoIPServerDoc, CDocument)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CVoIPServerDoc, CDocument)
END_DISPATCH_MAP()

// 참고: IID_IVoIPServer에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다.
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {8ad3ccfc-1604-4f49-bef6-cdf3b7649338}
static const IID IID_IVoIPServer =
{0x8ad3ccfc,0x1604,0x4f49,{0xbe,0xf6,0xcd,0xf3,0xb7,0x64,0x93,0x38}};

BEGIN_INTERFACE_MAP(CVoIPServerDoc, CDocument)
	INTERFACE_PART(CVoIPServerDoc, IID_IVoIPServer, Dispatch)
END_INTERFACE_MAP()


// CVoIPServerDoc 생성/소멸

CVoIPServerDoc::CVoIPServerDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

	EnableAutomation();

	AfxOleLockApp();
}

CVoIPServerDoc::~CVoIPServerDoc()
{
	AfxOleUnlockApp();
}

BOOL CVoIPServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CVoIPServerDoc serialization

void CVoIPServerDoc::Serialize(CArchive& ar)
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
void CVoIPServerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CVoIPServerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CVoIPServerDoc::SetSearchContent(const CString& value)
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

// CVoIPServerDoc 진단

#ifdef _DEBUG
void CVoIPServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVoIPServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVoIPServerDoc 명령
