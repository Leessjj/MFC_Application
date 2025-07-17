// MFCApplicationDoc.cpp: CMFCApplicationDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCApplication.h"
#endif

#include "MFCApplicationDoc.h"
#include "MFCApplicationView.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCApplicationDoc

IMPLEMENT_DYNCREATE(CMFCApplicationDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCApplicationDoc, CDocument)
	ON_COMMAND(ID_IMAGE_FLIP_HORIZONTAL, &CMFCApplicationDoc::OnImageFlipHorizontal)
	ON_COMMAND(ID_IMAGE_FLIP_VERTICAL, &CMFCApplicationDoc::OnImageFlipVertical)
END_MESSAGE_MAP()


// CMFCApplicationDoc 생성/소멸

CMFCApplicationDoc::CMFCApplicationDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CMFCApplicationDoc::~CMFCApplicationDoc()
{
}

BOOL CMFCApplicationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CMFCApplicationDoc serialization

void CMFCApplicationDoc::Serialize(CArchive& ar)
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
void CMFCApplicationDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CMFCApplicationDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMFCApplicationDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMFCApplicationDoc 진단

#ifdef _DEBUG
void CMFCApplicationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCApplicationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCApplicationDoc 명령

BOOL CMFCApplicationDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_image.Destroy(); // 기존 이미지 해제
	HRESULT hr = m_image.Load(lpszPathName); // 이미지 로드
	if (FAILED(hr)) {
		AfxMessageBox(_T("이미지 로드 실패"));
		return FALSE;
	}
	if (!m_channelR.IsNull()) m_channelR.Destroy();
	if (!m_channelG.IsNull()) m_channelG.Destroy();
	if (!m_channelB.IsNull()) m_channelB.Destroy();

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
		CView* pView = GetNextView(pos);
		CMFCApplicationView* pMyView = dynamic_cast<CMFCApplicationView*>(pView);
		if (pMyView) {
			pMyView->m_selectedChannel = CMFCApplicationView::CHANNEL_ORG;
		}
	}

	UpdateAllViews(NULL); // 화면 갱신 요청
	return TRUE;
}

BOOL CMFCApplicationDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (m_image.IsNull())
		return FALSE; // 저장할 이미지가 없음

	HRESULT hr = m_image.Save(lpszPathName); // 파일로 저장
	if (FAILED(hr)) {
		AfxMessageBox(_T("이미지 저장 실패"));
		return FALSE;
	}
	return TRUE;
}
void CMFCApplicationDoc::OnImageFlipHorizontal()
{
	if (m_image.IsNull()) return;

	int width = m_image.GetWidth();
	int height = m_image.GetHeight();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width / 2; ++x)
		{
			COLORREF left = m_image.GetPixel(x, y);
			COLORREF right = m_image.GetPixel(width - 1 - x, y);
			m_image.SetPixel(x, y, right);
			m_image.SetPixel(width - 1 - x, y, left);
		}
	}
	UpdateAllViews(NULL); // 화면 새로 그리기
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMFCApplicationDoc::OnImageFlipVertical()
{
	if (m_image.IsNull()) return;

	int width = m_image.GetWidth();
	int height = m_image.GetHeight();

	for (int y = 0; y < height / 2; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			COLORREF top = m_image.GetPixel(x, y);
			COLORREF bottom = m_image.GetPixel(x, height - 1 - y);
			m_image.SetPixel(x, y, bottom);
			m_image.SetPixel(x, height - 1 - y, top);
		}
	}
	UpdateAllViews(NULL); // 화면 새로 그리기
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMFCApplicationDoc::ExtractRGBChannel(char channel)
{
	int width = m_image.GetWidth();
	int height = m_image.GetHeight();

	// 채널별 이미지 초기화
	if (channel == 'R') {
		if (!m_channelR.IsNull()) m_channelR.Destroy();
		m_channelR.Create(width, height, 24);
	}
	if (channel == 'G') {
		if (!m_channelG.IsNull()) m_channelG.Destroy();
		m_channelG.Create(width, height, 24);
	}
	if (channel == 'B') {
		if (!m_channelB.IsNull()) m_channelB.Destroy();
		m_channelB.Create(width, height, 24);
	}

	for (int y = 0; y < height; ++y)
	{
		BYTE* pSrc = (BYTE*)m_image.GetPixelAddress(0, y);
		BYTE* pDst = nullptr;

		if (channel == 'R') pDst = (BYTE*)m_channelR.GetPixelAddress(0, y);
		if (channel == 'G') pDst = (BYTE*)m_channelG.GetPixelAddress(0, y);
		if (channel == 'B') pDst = (BYTE*)m_channelB.GetPixelAddress(0, y);

		for (int x = 0; x < width; ++x)
		{
			BYTE b = pSrc[x * 3 + 0];
			BYTE g = pSrc[x * 3 + 1];
			BYTE r = pSrc[x * 3 + 2];

			if (channel == 'R') {
				pDst[x * 3 + 0] = 0; // B
				pDst[x * 3 + 1] = 0; // G
				pDst[x * 3 + 2] = r; // R만 남김
			}
			if (channel == 'G') {
				pDst[x * 3 + 0] = 0;
				pDst[x * 3 + 1] = g;
				pDst[x * 3 + 2] = 0;
			}
			if (channel == 'B') {
				pDst[x * 3 + 0] = b;
				pDst[x * 3 + 1] = 0;
				pDst[x * 3 + 2] = 0;
			}
		}
	}
}

