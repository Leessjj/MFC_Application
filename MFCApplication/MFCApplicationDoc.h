
// MFCApplicationDoc.h: CMFCApplicationDoc 클래스의 인터페이스
//


#pragma once
#include <vector>

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
	int   m_imgW = 0;   // 실제 이미지 가로
	int   m_imgH = 0;   // 실제 이미지 세로

	// (2) 채널 분리(선택)용 버퍼
	BYTE* m_pChannelR = nullptr;
	BYTE* m_pChannelG = nullptr;
	BYTE* m_pChannelB = nullptr;

	// --- [1] 결함 영역 구조체/벡터 선언 ---
	struct DefectRegion {
		int x, y, w, h;
	};
	std::vector<DefectRegion> m_defectRegions;

	// --- [2] 결함 검출 함수 선언 ---
	void DetectDefects(int threshold = 40, int minSize = 10);
	void DetectStainRegions();
	struct StainRegion { int x, y, w, h; };
	std::vector<StainRegion> m_stainRegions;

	double m_stddev = 0;

	// 작업입니다.
public:
	// (3) 기존 작업 함수들은 동일하게 제공, 다만 버퍼 기반 구현!
	void ExtractRGBChannel(char channel);

	// (4) 추가: 버퍼 관리 및 BMP 입출력
	void AllocateImage(int width, int height, int channels = 3);
	void FreeImage();
	BOOL LoadBMP(LPCTSTR lpszPathName);
	BOOL SaveBMP(LPCTSTR lpszPathName);

	void ResizeCanvas(int newW, int newH);
	//필터
	void ApplyGrayscale();
	void ApplyGaussianBlur();
	void ApplySobelEdge();
	void ApplySepia();

	void PushUndo();
	void Undo();

	void FlipBuffer(const BYTE* src, BYTE* dst, int width, int height, bool flipH, bool flipV);
	// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName)override;
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
	//undo
	std::vector<std::vector<BYTE>> m_undoStack;

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};