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
#include "MainFrm.h"

#include <windows.h>   // RGB, GetRValue 등
#include <algorithm>   // std::swap


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


// --- 버퍼 관리 함수 ---
void CMFCApplicationDoc::FreeImage()
{
    if (m_pImage) { delete[] m_pImage; m_pImage = nullptr; }
    if (m_pChannelR) { delete[] m_pChannelR; m_pChannelR = nullptr; }
    if (m_pChannelG) { delete[] m_pChannelG; m_pChannelG = nullptr; }
    if (m_pChannelB) { delete[] m_pChannelB; m_pChannelB = nullptr; }
}

void CMFCApplicationDoc::AllocateImage(int width, int height, int channels)
{
    FreeImage();
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_pImage = new BYTE[width * height * channels];

    TRACE(_T("[DEBUG] AllocateImage: width=%d, height=%d, channels=%d, total bytes=%d\n"),
        width, height, channels, width * height * channels);
}
// --- BMP 파일 읽기 ---
BOOL CMFCApplicationDoc::LoadBMP(LPCTSTR lpszPathName)
{
    CFile file;
    if (!file.Open(lpszPathName, CFile::modeRead | CFile::typeBinary))
        return FALSE;

    BITMAPFILEHEADER header;
    BITMAPINFOHEADER info;
    file.Read(&header, sizeof(header));
    file.Read(&info, sizeof(info));
    if (header.bfType != 0x4D42 || info.biBitCount != 24)
    {
        file.Close();
        AfxMessageBox(_T("24bit BMP만 지원합니다."));
        return FALSE;
    }

    AllocateImage(info.biWidth, abs(info.biHeight), 3);
    file.Seek(header.bfOffBits, CFile::begin);

    int pad = (4 - (m_width * 3) % 4) % 4;
    bool bBottomUp = info.biHeight > 0;

    for (int y = 0; y < m_height; ++y)
    {
        int row = bBottomUp ? (m_height - 1 - y) : y;
        file.Read(m_pImage + row * m_width * 3, m_width * 3);
        BYTE dummy[4] = { 0, };
        file.Read(dummy, pad); // 반드시!!!
    }
    file.Close();
    return TRUE;
}



// --- BMP 파일 저장 ---
BOOL CMFCApplicationDoc::SaveBMP(LPCTSTR lpszPathName)
{
    if (!m_pImage) return FALSE;

    CFile file;
    if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
        return FALSE;

    int pad = (4 - (m_width * 3) % 4) % 4;
    int fileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
        + (m_width * 3 + pad) * m_height;

    BITMAPFILEHEADER header = { 0x4D42, (DWORD)fileSize, 0, 0,
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) };
    BITMAPINFOHEADER info = { sizeof(BITMAPINFOHEADER), m_width, m_height, 1, 24, 0, 0, 0, 0, 0, 0 };

    file.Write(&header, sizeof(header));
    file.Write(&info, sizeof(info));

    BYTE zeros[4] = { 0, };
    for (int y = m_height - 1; y >= 0; --y)
    {
        file.Write(m_pImage + y * m_width * 3, m_width * 3);
        file.Write(zeros, pad);
    }
    file.Close();
    return TRUE;
}


// ---- 명령 함수들 ----

BOOL CMFCApplicationDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm)
        pMainFrm->m_wndOutput.AddLog(lpszPathName);

    FreeImage();

    if (!LoadBMP(lpszPathName)) {
        AfxMessageBox(_T("이미지 로드 실패 (24bit BMP만 지원)"));
        return FALSE;
    }

    if (m_pChannelR) { delete[] m_pChannelR; m_pChannelR = nullptr; }
    if (m_pChannelG) { delete[] m_pChannelG; m_pChannelG = nullptr; }
    if (m_pChannelB) { delete[] m_pChannelB; m_pChannelB = nullptr; }

    POSITION pos = GetFirstViewPosition();
    while (pos != NULL) {
        CView* pView = GetNextView(pos);
        CMFCApplicationView* pMyView = dynamic_cast<CMFCApplicationView*>(pView);
        if (pMyView) {
            pMyView->m_selectedChannel = CMFCApplicationView::CHANNEL_ORG;
        }
    }

    UpdateAllViews(NULL);
    return TRUE;
}

BOOL CMFCApplicationDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm)
        pMainFrm->m_wndOutput.AddLog(_T("문서 저장"));
    if (!m_pImage)
        return FALSE;

    if (!SaveBMP(lpszPathName)) {
        AfxMessageBox(_T("이미지 저장 실패 (24bit BMP만 지원)"));
        return FALSE;
    }
    return TRUE;
}

// --- 좌우 반전 ---
void CMFCApplicationDoc::OnImageFlipHorizontal()
{
    if (!m_pImage) return;

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width / 2; ++x)
        {
            int idxL = (y * m_width + x) * 3;
            int idxR = (y * m_width + (m_width - 1 - x)) * 3;
            for (int c = 0; c < 3; ++c)
                std::swap(m_pImage[idxL + c], m_pImage[idxR + c]);
        }
    }
    UpdateAllViews(NULL);
}

// --- 상하 반전 ---
void CMFCApplicationDoc::OnImageFlipVertical()
{
    if (!m_pImage) return;

    for (int y = 0; y < m_height / 2; ++y)
    {
        int idxT = y * m_width * 3;
        int idxB = (m_height - 1 - y) * m_width * 3;
        for (int x = 0; x < m_width * 3; ++x)
            std::swap(m_pImage[idxT + x], m_pImage[idxB + x]);
    }
    UpdateAllViews(NULL);
}

// --- RGB 채널 추출 ---
void CMFCApplicationDoc::ExtractRGBChannel(char channel)
{
    if (!m_pImage) return;
    int width = m_width, height = m_height;
    BYTE*& pChannel =
        (channel == 'R') ? m_pChannelR :
        (channel == 'G') ? m_pChannelG :
        m_pChannelB;

    if (pChannel) { delete[] pChannel; pChannel = nullptr; }
    pChannel = new BYTE[width * height * 3];

    CString logMsg;
    if (channel == 'R') logMsg = _T("R채널 추출");
    else if (channel == 'G') logMsg = _T("G채널 추출");
    else if (channel == 'B') logMsg = _T("B채널 추출");
    CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm) pMainFrm->m_wndOutput.AddLog(logMsg);

    for (int i = 0; i < width * height; ++i)
    {
        BYTE b = m_pImage[i * 3 + 0];
        BYTE g = m_pImage[i * 3 + 1];
        BYTE r = m_pImage[i * 3 + 2];

        if (channel == 'R') {
            pChannel[i * 3 + 0] = 0;
            pChannel[i * 3 + 1] = 0;
            pChannel[i * 3 + 2] = r;
        }
        else if (channel == 'G') {
            pChannel[i * 3 + 0] = 0;
            pChannel[i * 3 + 1] = g;
            pChannel[i * 3 + 2] = 0;
        }
        else if (channel == 'B') {
            pChannel[i * 3 + 0] = b;
            pChannel[i * 3 + 1] = 0;
            pChannel[i * 3 + 2] = 0;
        }
    }
    UpdateAllViews(NULL);
}

