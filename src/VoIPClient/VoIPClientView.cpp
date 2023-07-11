
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

#include "session/UiController.h"

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
	ON_COMMAND(ID_CV_CONNECTION, &CVoIPClientView::OnConnection)
	ON_COMMAND(ID_CV_CHK_VAD, &CVoIPClientView::OnCheckedVAD)
	ON_COMMAND(ID_CV_CHK_ECHO, &CVoIPClientView::OnCheckedEcho)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	ON_WM_CREATE()
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

	return CView::PreCreateWindow(cs);
}

// CVoIPClientView 그리기

void CVoIPClientView::OnDraw(CDC* /*pDC*/)
{
	CVoIPClientDoc* pDoc = GetDocument();
	
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CRect rectClient;
	GetClientRect(rectClient);
	CPoint StartPos = rectClient.CenterPoint();

	// m_DisplayBox Size 
	int bmWidth = 1280;
	int bmHeight = 960;
	m_DisplayBox.SetWindowPos(NULL, StartPos.x - bmWidth / 2 + 150, StartPos.y - bmHeight / 2, bmWidth, bmHeight, SWP_DRAWFRAME);
	//m_DisplayBox.ShowWindow(SW_HIDE);
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
}

void CVoIPClientView::OnConnection()
{
	LOG_DEBUG("waiting something...");
	// @todo status 정보에 따라 request를 분기 해주ㅝ야 함.

	UiController::getInstance()->request_EndOrExitCall(this);
}

void CVoIPClientView::OnCheckedVAD()
{
	LOG_DEBUG("waiting something...");
	// @todo 인터페이스는 필요해 보임.

}

void CVoIPClientView::OnCheckedEcho()
{
	LOG_DEBUG("waiting something...");
	// @todo 인터페이스는 필요해 보임.

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


int CVoIPClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	GetClientRect(rectDummy);
	CPoint StartPos = rectDummy.CenterPoint();

	// 버튼을 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE;

	if (!m_btnConnection.Create(_T("Connection"), dwViewStyle, CRect(10 + 0 * 100, 10, 100 + 0 * 100, 36), this, ID_CV_CONNECTION))
	{
		TRACE0("Connection 버튼을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_btnVad.Create(_T("VAD"), dwViewStyle, CRect(10 + 1 * 100, 10, 100 + 1 * 100, 36), this, ID_CV_CHK_VAD))
	{
		TRACE0("VAD 버튼을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_btnEchoCancel.Create(_T("ECHO"), dwViewStyle, CRect(10 + 2 * 100, 10, 100 + 2 * 100, 36), this, ID_CV_CHK_ECHO))
	{
		TRACE0("ECHO 버튼을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_DisplayBox.Create(_T("Display"), dwViewStyle, CRect(10 + 3 * 100, 10, 100 + 3 * 100, 36), this, IDC_CLIENT_VIEW_DISPLAY))
	{
		TRACE0("Display을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	
	m_btnConnection.LoadBitmaps(IDB_BMP_DISCONNECTION, NULL, NULL, NULL);
	m_btnVad.LoadBitmaps(IDB_BMP_VAD_OFF, NULL, NULL, NULL);
	m_btnEchoCancel.LoadBitmaps(IDB_ECHO_OFF, NULL, NULL, NULL);

	m_btnConnection.SizeToContent();
	m_btnVad.SizeToContent();
	m_btnEchoCancel.SizeToContent();
	
	return 0;
}

LRESULT CVoIPClientView::processUiControlNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	default:
		break;
	}
	return LRESULT();
}

void CVoIPClientView::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_VIEW_DISPLAY, m_DisplayBox);

}
