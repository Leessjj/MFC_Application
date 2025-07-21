
// MFCApplicationDoc.h: CMFCApplicationDoc 클래스의 인터페이스
//


#pragma once


class CMFCApplicationDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CMFCApplicationDoc() noexcept;
	DECLARE_DYNCREATE(CMFCApplicationDoc)

	// 특성입니다.
public:
	BYTE* m_pImage = nullptr;         // 원본/주 이미지 버퍼(RGB)
	int   m_width = 0;                // 가로 픽셀
	int   m_height = 0;               // 세로 픽셀
	int   m_channels = 3;             // 채널(기본 3)

	// (2) 채널 분리(선택)용 버퍼
	BYTE* m_pChannelR = nullptr;
	BYTE* m_pChannelG = nullptr;
	BYTE* m_pChannelB = nullptr;

	// 작업입니다.
public:
	// (3) 기존 작업 함수들은 동일하게 제공, 다만 버퍼 기반 구현!
	void OnImageFlipHorizontal();
	void OnImageFlipVertical();
	void ExtractRGBChannel(char channel);

	// (4) 추가: 버퍼 관리 및 BMP 입출력
	void AllocateImage(int width, int height, int channels = 3);
	void FreeImage();
	BOOL LoadBMP(LPCTSTR lpszPathName);
	BOOL SaveBMP(LPCTSTR lpszPathName);

	// (5) (선택) 공백 이미지 생성 (그림판처럼 동작)
	void CreateBlankImage(int width, int height, COLORREF color = RGB(255, 255, 255));


	// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// 구현입니다.
public:
	virtual ~CMFCApplicationDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};