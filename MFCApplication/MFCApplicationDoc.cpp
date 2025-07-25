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

    m_imgW = info.biWidth;
    m_imgH = abs(info.biHeight);

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
    // 로그 출력(기존대로 유지)
    CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm)
        pMainFrm->m_wndOutput.AddLog(_T("문서 저장"));

    int nW = m_width, nH = m_height;
    if (nW <= 0 || nH <= 0 || !m_pImage)
        return FALSE;

    // ===== [1] 도형까지 합성된 이미지 생성 =====
    // (뷰 객체 얻기)
    CMFCApplicationView* pView = nullptr;
    POSITION pos = GetFirstViewPosition();
    while (pos) {
        CView* v = GetNextView(pos);
        if (v->IsKindOf(RUNTIME_CLASS(CMFCApplicationView))) {
            pView = (CMFCApplicationView*)v;
            break; // SDI라면 첫 번째 뷰만!
        }
    }
    if (!pView)
        return FALSE;

    // --- 24비트 DIB 버퍼 생성
    int stride = ((nW * 3) + 3) & ~3;
    std::vector<BYTE> dibBuf(stride * nH, 255); // 기본 흰색

    // [2] 선택된 채널 버퍼 얻기 (R/G/B/Org)
    BYTE* pBuf = m_pImage;
    switch (pView->m_selectedChannel) {
    case CMFCApplicationView::CHANNEL_R:
        if (m_pChannelR) pBuf = m_pChannelR; break;
    case CMFCApplicationView::CHANNEL_G:
        if (m_pChannelG) pBuf = m_pChannelG; break;
    case CMFCApplicationView::CHANNEL_B:
        if (m_pChannelB) pBuf = m_pChannelB; break;
    }

    // --- 이미지 버퍼를 DIB 버퍼로 복사
    for (int y = 0; y < nH; ++y)
        memcpy(&dibBuf[y * stride], pBuf + y * nW * 3, nW * 3);

    // --- 메모리 DC 생성 후, DIB 버퍼 → DC
    CClientDC desktopDC(NULL);
    CDC memDC;
    memDC.CreateCompatibleDC(&desktopDC);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&desktopDC, nW, nH);
    CBitmap* pOldBmp = memDC.SelectObject(&bitmap);

    // --- BITMAPINFO 세팅
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nW;
    bmi.bmiHeader.biHeight = -nH; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    ::SetDIBitsToDevice(
        memDC.GetSafeHdc(), 0, 0, nW, nH, 0, 0, 0, nH, dibBuf.data(), &bmi, DIB_RGB_COLORS
    );

    // --- [3] 도형까지 합성
    pView->DrawAllShapesToDC(&memDC);

    // --- [4] DC에 그려진 비트맵을 24비트 DIB 버퍼로 다시 추출
    ::GetDIBits(memDC.GetSafeHdc(), (HBITMAP)bitmap, 0, nH, dibBuf.data(), &bmi, DIB_RGB_COLORS);

    memDC.SelectObject(pOldBmp);

    // ===== [5] 24비트 BMP 파일로 저장 =====
    CFile file;
    if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
        return FALSE;

    int pad = (4 - (nW * 3) % 4) % 4;
    int fileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
        + (nW * 3 + pad) * nH;

    BITMAPFILEHEADER header = { 0x4D42, (DWORD)fileSize, 0, 0,
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) };
    BITMAPINFOHEADER info = { sizeof(BITMAPINFOHEADER), nW, nH, 1, 24, 0, 0, 0, 0, 0, 0 };

    file.Write(&header, sizeof(header));
    file.Write(&info, sizeof(info));
    BYTE zeros[4] = { 0, };
    for (int y = nH - 1; y >= 0; --y)
    {
        file.Write(dibBuf.data() + y * stride, nW * 3);
        file.Write(zeros, pad);
    }
    file.Close();

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
    int imgW = m_imgW, imgH = m_imgH;
    BYTE*& pChannel =
        (channel == 'R') ? m_pChannelR :
        (channel == 'G') ? m_pChannelG :
        m_pChannelB;

    if (pChannel) { delete[] pChannel; pChannel = nullptr; }
    pChannel = new BYTE[width * height * 3]; // 전체 도화지 사이즈는 유지

    // 도화지 전체를 일단 흰색(RGB=255)으로 초기화
    memset(pChannel, 255, width * height * 3);

    CString logMsg;
    if (channel == 'R') logMsg = _T("R채널 추출");
    else if (channel == 'G') logMsg = _T("G채널 추출");
    else if (channel == 'B') logMsg = _T("B채널 추출");
    CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm) pMainFrm->m_wndOutput.AddLog(logMsg);

    for (int y = 0; y < imgH; ++y)
    {
        for (int x = 0; x < imgW; ++x)
        {
            int idx = (y * width + x) * 3;
            BYTE b = m_pImage[idx + 0];
            BYTE g = m_pImage[idx + 1];
            BYTE r = m_pImage[idx + 2];
            if (channel == 'R') {
                pChannel[idx + 0] = 0;
                pChannel[idx + 1] = 0;
                pChannel[idx + 2] = r;
            }
            else if (channel == 'G') {
                pChannel[idx + 0] = 0;
                pChannel[idx + 1] = g;
                pChannel[idx + 2] = 0;
            }
            else if (channel == 'B') {
                pChannel[idx + 0] = b;
                pChannel[idx + 1] = 0;
                pChannel[idx + 2] = 0;
            }
        }
    }
    UpdateAllViews(NULL);
}

void CMFCApplicationDoc::ResizeCanvas(int newW, int newH)
{
    // 1. 새 도화지 버퍼 생성 (흰색)
    BYTE* newCanvas = new BYTE[newW * newH * 3];
    memset(newCanvas, 255, newW * newH * 3); // 전체 흰색

    // 2. 기존 이미지 복사 (겹치는 부분만)
    if (m_pImage) {
        int copyW = min(m_width, newW);
        int copyH = min(m_height, newH);
        for (int y = 0; y < copyH; ++y) {
            memcpy(
                newCanvas + (y * newW * 3),
                m_pImage + (y * m_width * 3),
                copyW * 3
            );
        }
        delete[] m_pImage;
    }
    m_pImage = newCanvas;

    // 3. 채널 버퍼(R/G/B)도 똑같이 새 크기로 확장/복사/초기화
    auto resizeChannel = [&](BYTE*& pChannel) {
        if (!pChannel) return;
        BYTE* newCh = new BYTE[newW * newH * 3];
        memset(newCh, 255, newW * newH * 3); // 전체 흰색
        int copyW = min(m_width, newW);
        int copyH = min(m_height, newH);
        for (int y = 0; y < copyH; ++y) {
            memcpy(
                newCh + (y * newW * 3),
                pChannel + (y * m_width * 3),
                copyW * 3
            );
        }
        delete[] pChannel;
        pChannel = newCh;
        };

    resizeChannel(m_pChannelR);
    resizeChannel(m_pChannelG);
    resizeChannel(m_pChannelB);

    // 4. 도화지 크기 갱신
    m_width = newW;
    m_height = newH;

    // 5. 모든 뷰에 스크롤/화면 갱신 알리기
    POSITION pos = GetFirstViewPosition();
    while (pos)
    {
        CView* pView = GetNextView(pos);
        if (pView->IsKindOf(RUNTIME_CLASS(CScrollView))) {
            ((CScrollView*)pView)->SetScrollSizes(MM_TEXT, CSize(m_width, m_height));
        }
        pView->Invalidate(FALSE);
    }
}

void CMFCApplicationDoc::ApplyGrayscale()
{
    if (!m_pImage) return;
    int nPix = m_width * m_height;
    for (int i = 0; i < nPix; ++i)
    {
        BYTE b = m_pImage[i * 3 + 0];
        BYTE g = m_pImage[i * 3 + 1];
        BYTE r = m_pImage[i * 3 + 2];
        // 가중치 방식이 더 자연스러움 (0.299R + 0.587G + 0.114B)
        BYTE gray = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b + 0.5);
        m_pImage[i * 3 + 0] = gray;
        m_pImage[i * 3 + 1] = gray;
        m_pImage[i * 3 + 2] = gray;
    }
    UpdateAllViews(NULL);
}
void CMFCApplicationDoc::ApplyGaussianBlur()
{
    if (!m_pImage) return;
    int w = m_width, h = m_height;
    // 3x3 가우시안 커널 (합 = 16)
    const int kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    // 임시 버퍼에 결과 저장 (원본 덮어쓰기 방지)
    BYTE* pOut = new BYTE[w * h * 3];
    memcpy(pOut, m_pImage, w * h * 3);

    for (int y = 1; y < h - 1; ++y)
    {
        for (int x = 1; x < w - 1; ++x)
        {
            int sumB = 0, sumG = 0, sumR = 0;
            int sumK = 0;
            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int px = x + kx, py = y + ky;
                    int k = kernel[ky + 1][kx + 1];
                    int idx = (py * w + px) * 3;
                    sumB += m_pImage[idx + 0] * k;
                    sumG += m_pImage[idx + 1] * k;
                    sumR += m_pImage[idx + 2] * k;
                    sumK += k;
                }
            }
            int outIdx = (y * w + x) * 3;
            pOut[outIdx + 0] = (BYTE)(sumB / sumK);
            pOut[outIdx + 1] = (BYTE)(sumG / sumK);
            pOut[outIdx + 2] = (BYTE)(sumR / sumK);
        }
    }

    // 테두리 픽셀은 원본 유지 (성능·간결함 위함)
    memcpy(m_pImage, pOut, w * h * 3);
    delete[] pOut;

    UpdateAllViews(NULL);
}

void CMFCApplicationDoc::ApplySobelEdge()
{
    if (!m_pImage) return;
    int w = m_width, h = m_height;
    // 그레이스케일로 먼저 변환
    std::vector<BYTE> gray(w * h, 0);
    for (int i = 0; i < w * h; ++i)
    {
        BYTE b = m_pImage[i * 3 + 0];
        BYTE g = m_pImage[i * 3 + 1];
        BYTE r = m_pImage[i * 3 + 2];
        gray[i] = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b + 0.5);
    }

    // 결과 버퍼
    std::vector<BYTE> out(w * h, 0);

    // 소벨 커널
    int Gx[3][3] = {
        { -1, 0, +1 },
        { -2, 0, +2 },
        { -1, 0, +1 }
    };
    int Gy[3][3] = {
        { +1, +2, +1 },
        {  0,  0,  0 },
        { -1, -2, -1 }
    };

    // 소벨 연산
    for (int y = 1; y < h - 1; ++y)
    {
        for (int x = 1; x < w - 1; ++x)
        {
            int gx = 0, gy = 0;
            for (int ky = -1; ky <= 1; ++ky)
            {
                for (int kx = -1; kx <= 1; ++kx)
                {
                    int px = x + kx;
                    int py = y + ky;
                    int val = gray[py * w + px];
                    gx += val * Gx[ky + 1][kx + 1];
                    gy += val * Gy[ky + 1][kx + 1];
                }
            }
            int mag = (int)sqrt((double)(gx * gx + gy * gy));
            if (mag > 255) mag = 255;
            out[y * w + x] = (BYTE)mag;
        }
    }

    // 결과를 m_pImage에 적용 (흑백, R=G=B)
    for (int i = 0; i < w * h; ++i)
    {
        m_pImage[i * 3 + 0] = out[i];
        m_pImage[i * 3 + 1] = out[i];
        m_pImage[i * 3 + 2] = out[i];
    }
    UpdateAllViews(NULL);
}

void CMFCApplicationDoc::ApplySepia()
{
    if (!m_pImage) return;
    int nPix = m_width * m_height;
    for (int i = 0; i < nPix; ++i)
    {
        BYTE b = m_pImage[i * 3 + 0];
        BYTE g = m_pImage[i * 3 + 1];
        BYTE r = m_pImage[i * 3 + 2];

        int tr = (int)(0.393 * r + 0.769 * g + 0.189 * b);
        int tg = (int)(0.349 * r + 0.686 * g + 0.168 * b);
        int tb = (int)(0.272 * r + 0.534 * g + 0.131 * b);

        if (tr > 255) tr = 255;
        if (tg > 255) tg = 255;
        if (tb > 255) tb = 255;

        m_pImage[i * 3 + 0] = (BYTE)tb;
        m_pImage[i * 3 + 1] = (BYTE)tg;
        m_pImage[i * 3 + 2] = (BYTE)tr;
    }
    UpdateAllViews(NULL);
}
