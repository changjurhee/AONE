
// VoIPClientView.cpp: CVoIPClientView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "VoIPClient.h"
#endif

#include "VoIPClientDoc.h"
#include "VoIPClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVoIPClientView

IMPLEMENT_DYNCREATE(CVoIPClientView, CView)

BEGIN_MESSAGE_MAP(CVoIPClientView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CVoIPClientView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_CUT, &CVoIPClientView::OnEditCut)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CVoIPClientView 생성/소멸

CVoIPClientView::CVoIPClientView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	LOG_DEBUG("waiting something...");

}

CVoIPClientView::~CVoIPClientView()
{
}

BOOL CVoIPClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	LOG_DEBUG("waiting something...");

	return CView::PreCreateWindow(cs);
}

// CVoIPClientView 그리기

void CVoIPClientView::OnDraw(CDC* /*pDC*/)
{
	CVoIPClientDoc* pDoc = GetDocument();
	
	LOG_DEBUG("waiting something...");

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CVoIPClientView 인쇄


void CVoIPClientView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CVoIPClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CVoIPClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CVoIPClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CVoIPClientView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CVoIPClientView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, FALSE);
#endif
}

void CVoIPClientView::OnEditCut()
{
	LOG_DEBUG("waiting something...");
}

// CVoIPClientView 진단

#ifdef _DEBUG
void CVoIPClientView::AssertValid() const
{
	CView::AssertValid();
}

void CVoIPClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVoIPClientDoc* CVoIPClientView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVoIPClientDoc)));
	return (CVoIPClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CVoIPClientView 메시지 처리기

void CVoIPClientView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	
	CRect rectClient;
	GetClientRect(rectClient);
	CPoint StartPos = rectClient.CenterPoint();

	CDC MemDC;
	BITMAP bmpInfo;

	// 화면 DC와 호환되는 메모리 DC를 생성
	MemDC.CreateCompatibleDC(&dc);

	// 비트맵 리소스 로딩
	CBitmap bmp;
	CBitmap* pOldBmp = NULL;
	bmp.LoadBitmapW(IDB_MASK);

	// 로딩된 비트맵 정보 확인
	bmp.GetBitmap(&bmpInfo);

	// 메모리 DC에 선택
	pOldBmp = MemDC.SelectObject(&bmp);

	// 메모리 DC에 들어 있는 비트맵을 화면 DC로 복사하여 출력
	dc.BitBlt(StartPos.x - bmpInfo.bmWidth / 2, StartPos.y - bmpInfo.bmHeight / 2, bmpInfo.bmWidth, bmpInfo.bmHeight, &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject(pOldBmp);
}


void CVoIPClientView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
}
