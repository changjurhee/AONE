
// VoIPClientDoc.h: CVoIPClientDoc 클래스의 인터페이스
//

#pragma once

#include "AccountLoginDlg.h"
#include "ManageUserAccountDlg.h"

class CVoIPClientDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CVoIPClientDoc() noexcept;
	DECLARE_DYNCREATE(CVoIPClientDoc)

// 특성입니다.
public:
	std::shared_ptr<CAccountLoginDlg::userInfo> m_spUserInfo;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CVoIPClientDoc();

	void UserLogIn();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

	// 생성된 OLE 디스패치 맵 함수

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
