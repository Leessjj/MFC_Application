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
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCApplicationDoc

IMPLEMENT_DYNCREATE(CMFCApplicationDoc, CDocument)


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
    m_defectRegions.clear();
    m_stddev = 0;

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
// MFCApplicationDoc.cpp

void CMFCApplicationDoc::FlipBuffer(const BYTE* src, BYTE* dst, int width, int height, bool flipH, bool flipV)
{
    for (int y = 0; y < height; ++y) {
        int newY = flipV ? (height - 1 - y) : y;
        for (int x = 0; x < width; ++x) {
            int newX = flipH ? (width - 1 - x) : x;
            for (int c = 0; c < 3; ++c) {
                dst[(newY * width + newX) * 3 + c] = src[(y * width + x) * 3 + c];
            }
        }
    }
}


BOOL CMFCApplicationDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm)
        pMainFrm->m_wndOutput.AddLog(_T("문서 저장"));

    int nW = m_width, nH = m_height;
    if (nW <= 0 || nH <= 0 || !m_pImage)
        return FALSE;

    // ① 현재 뷰의 반전 옵션을 읽음
    CMFCApplicationView* pView = nullptr;
    POSITION pos = GetFirstViewPosition();
    while (pos) {
        CView* v = GetNextView(pos);
        if (v->IsKindOf(RUNTIME_CLASS(CMFCApplicationView))) {
            pView = (CMFCApplicationView*)v;
            break;
        }
    }
    if (!pView)
        return FALSE;

    // ② [NEW] 반전된 이미지 버퍼 만들기
    std::vector<BYTE> flippedBuf(nW * nH * 3);
    FlipBuffer(m_pImage, flippedBuf.data(), nW, nH, pView->m_bFlipH, pView->m_bFlipV);

    // ③ DIB 버퍼로 복사 (stride 맞추기)
    int stride = ((nW * 3) + 3) & ~3;
    std::vector<BYTE> dibBuf(stride * nH, 255);
    for (int y = 0; y < nH; ++y)
        memcpy(&dibBuf[y * stride], flippedBuf.data() + y * nW * 3, nW * 3);

    // ④ 메모리 DC 만들고 DIB 출력
    CClientDC desktopDC(NULL);
    CDC memDC;
    memDC.CreateCompatibleDC(&desktopDC);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&desktopDC, nW, nH);
    CBitmap* pOldBmp = memDC.SelectObject(&bitmap);

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

    // ⑤ [중요] 도형 합성 (반전 옵션 자동 적용)
    pView->DrawAllShapesToDC(&memDC);

    // ⑥ DC에서 다시 DIB 버퍼로 추출
    ::GetDIBits(memDC.GetSafeHdc(), (HBITMAP)bitmap, 0, nH, dibBuf.data(), &bmi, DIB_RGB_COLORS);

    memDC.SelectObject(pOldBmp);

    // ⑦ BMP 파일로 저장
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


// --- RGB 채널 추출 ---
// MFCApplicationDoc.cpp

void CMFCApplicationDoc::ExtractRGBChannel(char channel)
{
    if (!m_pImage) return;

    PushUndo(); // ★ 이 한 줄로 Undo 100% 지원

    int width = m_width, height = m_height;
    int imgW = m_imgW, imgH = m_imgH;

    CString logMsg;
    if (channel == 'R') logMsg = _T("도화지: R채널로 추출");
    else if (channel == 'G') logMsg = _T("도화지: G채널로 추출");
    else if (channel == 'B') logMsg = _T("도화지: B채널로 추출");

    CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm) pMainFrm->m_wndOutput.AddLog(logMsg);

    // 도화지 픽셀을 직접 해당 채널값만 남기도록 덮어쓰기
    for (int y = 0; y < imgH; ++y)
    {
        for (int x = 0; x < imgW; ++x)
        {
            int idx = (y * width + x) * 3;
            BYTE b = m_pImage[idx + 0];
            BYTE g = m_pImage[idx + 1];
            BYTE r = m_pImage[idx + 2];

            if (channel == 'R') {
                m_pImage[idx + 0] = 0;
                m_pImage[idx + 1] = 0;
                m_pImage[idx + 2] = r;
            }
            else if (channel == 'G') {
                m_pImage[idx + 0] = 0;
                m_pImage[idx + 1] = g;
                m_pImage[idx + 2] = 0;
            }
            else if (channel == 'B') {
                m_pImage[idx + 0] = b;
                m_pImage[idx + 1] = 0;
                m_pImage[idx + 2] = 0;
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
            ((CScrollView*)pView)->SetScrollSizes(MM_TEXT, CSize(
                int(m_width * ((CMFCApplicationView*)pView)->m_zoom),
                int(m_height * ((CMFCApplicationView*)pView)->m_zoom)
            ));
        }
        pView->Invalidate(FALSE);
    }

}

void CMFCApplicationDoc::ApplyGrayscale()
{
    PushUndo();
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
    PushUndo();
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
    PushUndo();
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
    PushUndo();
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

void CMFCApplicationDoc::PushUndo()
{
    if (!m_pImage) return;
    // 최대 스택 크기 제한(10단계 등) - 옵션
    const size_t MAX_UNDO = 10;
    if (m_undoStack.size() >= MAX_UNDO)
        m_undoStack.erase(m_undoStack.begin()); // 가장 오래된 것 삭제

    std::vector<BYTE> backup(m_pImage, m_pImage + m_width * m_height * 3);
    m_undoStack.push_back(std::move(backup));
}
void CMFCApplicationDoc::Undo()
{
    if (m_undoStack.empty()) return;
    std::vector<BYTE>& backup = m_undoStack.back();
    memcpy(m_pImage, backup.data(), m_width * m_height * 3);
    m_undoStack.pop_back();
    UpdateAllViews(NULL);
}

void CMFCApplicationDoc::DetectDefects(int diffThres, int minSize)
{
    m_defectRegions.clear();
    if (!m_pImage) return;
    int w = m_width, h = m_height;

    // 1. Gray 변환
    std::vector<BYTE> gray(w * h);
    for (int i = 0; i < w * h; ++i) {
        BYTE b = m_pImage[i * 3 + 0];
        BYTE g = m_pImage[i * 3 + 1];
        BYTE r = m_pImage[i * 3 + 2];
        gray[i] = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b + 0.5);
    }

    // 2. 국부 평균(7x7 영역) 구하기
    int ksize = 3; // 3이면 7x7영역, 2면 5x5
    std::vector<BYTE> mask(w * h, 0);
    for (int y = ksize; y < h - ksize; ++y) {
        for (int x = ksize; x < w - ksize; ++x) {
            int sum = 0, cnt = 0;
            for (int dy = -ksize; dy <= ksize; ++dy)
                for (int dx = -ksize; dx <= ksize; ++dx) {
                    sum += gray[(y + dy) * w + (x + dx)];
                    ++cnt;
                }
            int avg = sum / cnt;
            if (abs(gray[y * w + x] - avg) > diffThres) // diffThres: 민감도 8~20 추천
                mask[y * w + x] = 255;
        }
    }

    // 3. 미세점(1~5픽셀) 연결된 점 라벨링
    for (int y = ksize; y < h - ksize; ++y) {
        for (int x = ksize; x < w - ksize; ++x) {
            if (mask[y * w + x] == 255) {
                // 주변 확장 안 하고 그냥 작은 박스
                m_defectRegions.push_back({ x - 1, y - 1, 3, 3 });
            }
        }
    }
    /*CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrm) {
        CString msg;
        msg.Format(_T("=== 검출된 Defect 총 %d개 ==="), (int)m_defectRegions.size());
        pMainFrm->m_wndOutput.AddLog(msg);
        for (size_t i = 0; i < m_defectRegions.size(); ++i) {
            const auto& reg = m_defectRegions[i];
            msg.Format(_T("Defect %d: x=%d, y=%d, w=%d, h=%d"),
                (int)i + 1, reg.x, reg.y, reg.w, reg.h);
            pMainFrm->m_wndOutput.AddLog(msg);
        }
    }*/
}
