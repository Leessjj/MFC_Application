
// MFCApplicationView.cpp: CMFCApplicationView 클래스의 구현
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
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplicationView

IMPLEMENT_DYNCREATE(CMFCApplicationView, CScrollView)

BEGIN_MESSAGE_MAP(CMFCApplicationView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCApplicationView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(RChannel, &CMFCApplicationView::OnViewChannelR)
	ON_COMMAND(GChannel, &CMFCApplicationView::OnViewChannelG)
	ON_COMMAND(BChannel, &CMFCApplicationView::OnViewChannelB)

	ON_COMMAND(ID_IMAGE_FLIP_HORIZONTAL, &CMFCApplicationView::OnFlipHorizontal)
	ON_COMMAND(ID_IMAGE_FLIP_VERTICAL, &CMFCApplicationView::OnFlipVertical)

	ON_COMMAND(ID_DRAW_LINE, &CMFCApplicationView::OnDrawLine)
	ON_COMMAND(ID_DRAW_RECT, &CMFCApplicationView::OnDrawRect)
	ON_COMMAND(ID_DRAW_ELLIPSE, &CMFCApplicationView::OnDrawEllipse)
	ON_COMMAND(ID_DRAW_FREE, &CMFCApplicationView::OnDrawFreehand)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_VIEW_SAVEASIMAGE, &CMFCApplicationView::OnViewSaveasimage)

	ON_MESSAGE(WM_USER + 100, &CMFCApplicationView::OnOpenImageFileFromNet)
	ON_MESSAGE(WM_USER + 101, &CMFCApplicationView::OnSaveImageFromNet)
	ON_MESSAGE(WM_USER + 102, &CMFCApplicationView::OnFlipHFromNet)
	ON_MESSAGE(WM_USER + 103, &CMFCApplicationView::OnFlipVFromNet)
	ON_MESSAGE(WM_USER + 105, &CMFCApplicationView::OnChannelRFromNet)
	ON_MESSAGE(WM_USER + 106, &CMFCApplicationView::OnChannelGFromNet)
	ON_MESSAGE(WM_USER + 107, &CMFCApplicationView::OnChannelBFromNet)
	ON_MESSAGE(WM_USER + 108, &CMFCApplicationView::OnDrawLineFromNet)
	ON_MESSAGE(WM_USER + 109, &CMFCApplicationView::OnDrawRectFromNet)
	ON_MESSAGE(WM_USER + 110, &CMFCApplicationView::OnDrawEllipseFromNet)
	ON_MESSAGE(WM_USER + 111, &CMFCApplicationView::OnSaveAllFromNet)

	//도형,선 색상
	ON_BN_CLICKED(IDC_BTN_FILLCOLOR, &CMFCApplicationView::OnBnClickedBtnFillColor)
	ON_BN_CLICKED(IDC_BTN_BORDERCOLOR, &CMFCApplicationView::OnBnClickedBtnBorderColor)
	ON_MESSAGE(WM_USER + 112, &CMFCApplicationView::OnSetFillColor)
	ON_MESSAGE(WM_USER + 200, &CMFCApplicationView::OnSetBorderColor)

	//필터
	ON_MESSAGE(WM_USER + 201, &CMFCApplicationView::OnGrayscale)
	ON_MESSAGE(WM_USER + 202, &CMFCApplicationView::OnBlur)
	ON_MESSAGE(WM_USER + 203, &CMFCApplicationView::OnSobel)
	ON_MESSAGE(WM_USER + 204, &CMFCApplicationView::OnSepia)

	//검사
	ON_MESSAGE(WM_USER + 205, &CMFCApplicationView::OnDefects)
	ON_MESSAGE(WM_USER + 206, &CMFCApplicationView::OnNoise)
	ON_MESSAGE(WM_USER + 207, &CMFCApplicationView::OnStain)

	ON_COMMAND(ID_FILE_SAVE_AS, &CMFCApplicationView::OnFileSaveAs)
	ON_COMMAND(ID_FILTER_GRAYSCALE, &CMFCApplicationView::OnFilterGrayscale)
	ON_COMMAND(ID_FILTER_GAUSSIANBLUR, &CMFCApplicationView::OnFilterGaussianblur)
	ON_COMMAND(ID_FILTER_SOBELEDGE, &CMFCApplicationView::OnFilterSobeledge)
	ON_COMMAND(ID_FILTER_SEPIA, &CMFCApplicationView::OnFilterSepia)
	ON_COMMAND(ID_EDIT_UNDO, &CMFCApplicationView::OnEditUndo)

	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_DETECT_DEFECTS, &CMFCApplicationView::OnDetectDefects)
	ON_COMMAND(ID_CHECK_NOISE, &CMFCApplicationView::OnCheckNoise)
	ON_COMMAND(ID_DETECT_STAIN, &CMFCApplicationView::OnDetectStain)
END_MESSAGE_MAP()

// CMFCApplicationView 생성/소멸

CMFCApplicationView::CMFCApplicationView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMFCApplicationView::~CMFCApplicationView()
{
}

BOOL CMFCApplicationView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}

void CMFCApplicationView::OnDraw(CDC* pDC)
{
	// 1. 더블버퍼용 DC/Bitmap 준비
	CRect clientRect;
	GetClientRect(&clientRect);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&memBmp);

	// 2. 기존 코드의 pDC를 memDC로 "그대로" 바꿔서 사용
	memDC.FillSolidRect(clientRect, RGB(211, 211, 211));

	CMFCApplicationDoc* pDoc = GetDocument();
	if (!pDoc) {
		// 더블버퍼 복사
		pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBmp);
		return;
	}

	BYTE* pBuf = pDoc->m_pImage;
	int nW = pDoc->m_width, nH = pDoc->m_height;
	switch (m_selectedChannel) {
	case CHANNEL_R: if (pDoc->m_pChannelR) pBuf = pDoc->m_pChannelR; break;
	case CHANNEL_G: if (pDoc->m_pChannelG) pBuf = pDoc->m_pChannelG; break;
	case CHANNEL_B: if (pDoc->m_pChannelB) pBuf = pDoc->m_pChannelB; break;
	default: break;
	}
	if (!pBuf || nW == 0 || nH == 0) {
		pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBmp);
		return;
	}

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nW;
	bmi.bmiHeader.biHeight = -nH; // Top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	int stride = ((nW * 3) + 3) & ~3;
	std::vector<BYTE> dibBuf(stride * nH, 0);
	for (int y = 0; y < nH; ++y)
		memcpy(&dibBuf[y * stride], pBuf + y * nW * 3, nW * 3);

	CDC imgDC;
	imgDC.CreateCompatibleDC(&memDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&memDC, nW, nH);
	CBitmap* pOldBmpImg = imgDC.SelectObject(&bmp);

	::SetDIBitsToDevice(
		imgDC.GetSafeHdc(), 0, 0, nW, nH, 0, 0, 0, nH, dibBuf.data(), &bmi, DIB_RGB_COLORS
	);

	int drawW = int(nW * m_zoom);
	int drawH = int(nH * m_zoom);
	memDC.SetStretchBltMode(HALFTONE);

	if (m_bFlipH && m_bFlipV)
		memDC.StretchBlt(drawW - 1, drawH - 1, -drawW, -drawH, &imgDC, 0, 0, nW, nH, SRCCOPY);
	else if (m_bFlipH)
		memDC.StretchBlt(drawW - 1, 0, -drawW, drawH, &imgDC, 0, 0, nW, nH, SRCCOPY);
	else if (m_bFlipV)
		memDC.StretchBlt(0, drawH - 1, drawW, -drawH, &imgDC, 0, 0, nW, nH, SRCCOPY);
	else
		memDC.StretchBlt(0, 0, drawW, drawH, &imgDC, 0, 0, nW, nH, SRCCOPY);

	imgDC.SelectObject(pOldBmpImg);

	auto FlipPoint = [this, nW, nH](const CPoint& pt) -> CPoint {
		CPoint res = pt;
		if (m_bFlipH) res.x = nW - 1 - res.x;
		if (m_bFlipV) res.y = nH - 1 - res.y;
		return res;
		};

	// --- 도형 및 미리보기 등 (좌표에 zoom 곱해서 출력)
	// 기존 코드의 pDC → memDC로 바꿔서 쓴다!
	for (const auto& shape : m_shapes)
	{
		CPen pen(PS_SOLID, shape.borderWidth, shape.borderColor);
		CPen* pOldPen = memDC.SelectObject(&pen);

		if (shape.type == DRAW_FREEHAND && shape.freehandPts.size() >= 2)
		{
			auto pt = FlipPoint(shape.freehandPts[0]);
			pt.x = int(pt.x * m_zoom); pt.y = int(pt.y * m_zoom);
			memDC.MoveTo(pt);
			for (size_t i = 1; i < shape.freehandPts.size(); ++i) {
				auto pt2 = FlipPoint(shape.freehandPts[i]);
				pt2.x = int(pt2.x * m_zoom); pt2.y = int(pt2.y * m_zoom);
				memDC.LineTo(pt2);
			}
		}
		else
		{
			CPoint pt1 = FlipPoint(shape.start);
			CPoint pt2 = FlipPoint(shape.end);
			pt1.x = int(pt1.x * m_zoom); pt1.y = int(pt1.y * m_zoom);
			pt2.x = int(pt2.x * m_zoom); pt2.y = int(pt2.y * m_zoom);

			CBrush brush(shape.fillColor);
			CBrush* pOldBrush = memDC.SelectObject(&brush);

			switch (shape.type)
			{
			case DRAW_LINE:    memDC.MoveTo(pt1); memDC.LineTo(pt2); break;
			case DRAW_RECT:    memDC.Rectangle(CRect(pt1, pt2)); break;
			case DRAW_ELLIPSE: memDC.Ellipse(CRect(pt1, pt2)); break;
			default: break;
			}
			memDC.SelectObject(pOldBrush);
		}
		memDC.SelectObject(pOldPen);
	}

	// --- 미리보기 도형 --- (드래그 중)
	if (m_bDrawing)
	{
		CPen pen(PS_SOLID, m_curBorderWidth, m_curBorderColor);
		CPen* pOldPen = memDC.SelectObject(&pen);

		if (m_drawType == DRAW_FREEHAND && m_tempFreehandPts.size() >= 2)
		{
			auto pt = FlipPoint(m_tempFreehandPts[0]);
			pt.x = int(pt.x * m_zoom); pt.y = int(pt.y * m_zoom);
			memDC.MoveTo(pt);
			for (size_t i = 1; i < m_tempFreehandPts.size(); ++i) {
				auto pt2 = FlipPoint(m_tempFreehandPts[i]);
				pt2.x = int(pt2.x * m_zoom); pt2.y = int(pt2.y * m_zoom);
				memDC.LineTo(pt2);
			}
		}
		else if (m_drawType == DRAW_LINE || m_drawType == DRAW_RECT || m_drawType == DRAW_ELLIPSE)
		{
			CPoint pt1 = FlipPoint(m_startPoint);
			CPoint pt2 = FlipPoint(m_endPoint);
			pt1.x = int(pt1.x * m_zoom); pt1.y = int(pt1.y * m_zoom);
			pt2.x = int(pt2.x * m_zoom); pt2.y = int(pt2.y * m_zoom);

			CBrush brush(m_curFillColor);
			CBrush* pOldBrush = memDC.SelectObject(&brush);

			switch (m_drawType)
			{
			case DRAW_LINE:    memDC.MoveTo(pt1); memDC.LineTo(pt2); break;
			case DRAW_RECT:    memDC.Rectangle(CRect(pt1, pt2)); break;
			case DRAW_ELLIPSE: memDC.Ellipse(CRect(pt1, pt2)); break;
			}
			memDC.SelectObject(pOldBrush);
		}
		memDC.SelectObject(pOldPen);
	}

	// 이하 핸들, 결함박스 등도 전부 pDC → memDC로 바꿔서 사용

	int canvasW = int(pDoc->m_width * m_zoom);
	int canvasH = int(pDoc->m_height * m_zoom);
	const int HANDLE_SIZE = 10; // 고정

	memDC.FillSolidRect(canvasW - HANDLE_SIZE / 2, canvasH - HANDLE_SIZE / 2, HANDLE_SIZE, HANDLE_SIZE, RGB(0, 0, 0));
	memDC.FillSolidRect(canvasW - HANDLE_SIZE, canvasH - HANDLE_SIZE / 2, HANDLE_SIZE, HANDLE_SIZE, RGB(0, 0, 0));
	memDC.FillSolidRect(canvasW - HANDLE_SIZE / 2, canvasH - HANDLE_SIZE, HANDLE_SIZE, HANDLE_SIZE, RGB(0, 0, 0));

	if (m_bResizing) {
		CPen pen(PS_DOT, 1, RGB(0, 0, 255));
		CPen* pOldPen = memDC.SelectObject(&pen);
		memDC.Rectangle(0, 0, int(m_resizePreviewW * m_zoom), int(m_resizePreviewH * m_zoom));
		memDC.SelectObject(pOldPen);
	}

	if (!pDoc->m_defectRegions.empty()) {
		CPen defectPen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen* pOldPen = memDC.SelectObject(&defectPen);

		CBrush* pOldBrush = (CBrush*)memDC.SelectStockObject(NULL_BRUSH);
		for (const auto& reg : pDoc->m_defectRegions)
		{
			int x1 = int(reg.x * m_zoom), y1 = int(reg.y * m_zoom);
			int x2 = int((reg.x + reg.w) * m_zoom), y2 = int((reg.y + reg.h) * m_zoom);
			memDC.Rectangle(x1, y1, x2, y2);
		}
		memDC.SelectObject(pOldPen);
	}

	if (pDoc->m_stddev > 0) {
		bool isNoiseOK = (pDoc->m_stddev < 15.0);

		CString resultText = isNoiseOK ? _T("PASS") : _T("FAIL");
		COLORREF resultColor = isNoiseOK ? RGB(0, 180, 0) : RGB(220, 0, 0);

		CFont bigFont;
		bigFont.CreatePointFont(240, _T("맑은 고딕"));
		CFont* pOldFont = memDC.SelectObject(&bigFont);

		CRect resultRect(30, 30, 300, 120);
		memDC.SetTextColor(resultColor);
		memDC.SetBkMode(TRANSPARENT);
		memDC.DrawText(resultText, &resultRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		memDC.SelectObject(pOldFont);
	}
	if (!pDoc->m_stainRegions.empty()) {
		CPen stainPen(PS_SOLID, 1, RGB(255, 0, 0));
		CPen* pOldPen = memDC.SelectObject(&stainPen);

		CBrush* pOldBrush = (CBrush*)memDC.SelectStockObject(NULL_BRUSH);
		int expand = int(50 * m_zoom);

		for (const auto& reg : pDoc->m_stainRegions) {
			int x1 = int(reg.x * m_zoom) - expand;
			int y1 = int(reg.y * m_zoom) - expand;
			int x2 = int((reg.x + reg.w) * m_zoom) + expand;
			int y2 = int((reg.y + reg.h) * m_zoom) + expand;
			memDC.Rectangle(x1, y1, x2, y2);
		}
		memDC.SelectObject(pOldBrush);
		memDC.SelectObject(pOldPen);
	}

	// --- 실제화면에 복사!
	pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBmp);
}

// CMFCApplicationView 인쇄
void CMFCApplicationView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCApplicationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMFCApplicationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMFCApplicationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMFCApplicationView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCApplicationView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCApplicationView 진단

#ifdef _DEBUG
void CMFCApplicationView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplicationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplicationDoc* CMFCApplicationView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplicationDoc)));
	return (CMFCApplicationDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplicationView 메시지 처리기

void CMFCApplicationView::OnViewChannelR()
{
	m_selectedChannel = CHANNEL_R;
	GetDocument()->ExtractRGBChannel('R');
	Invalidate();
}
void CMFCApplicationView::OnViewChannelG()
{
	m_selectedChannel = CHANNEL_G;
	GetDocument()->ExtractRGBChannel('G');
	Invalidate();
}
void CMFCApplicationView::OnViewChannelB()
{

	m_selectedChannel = CHANNEL_B;
	GetDocument()->ExtractRGBChannel('B');
	Invalidate();
}

void CMFCApplicationView::OnDrawLine()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("직선 선택"));
	m_drawType = DRAW_LINE;
}

void CMFCApplicationView::OnDrawRect()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("사각형 선택"));
	m_drawType = DRAW_RECT;
}

void CMFCApplicationView::OnDrawEllipse()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("원 선택"));
	m_drawType = DRAW_ELLIPSE;
}
void CMFCApplicationView::OnDrawFreehand()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("펜 선택"));
	m_drawType = DRAW_FREEHAND;
}

CPoint CMFCApplicationView::ViewToImage(const CPoint& pt, double zoom, bool flipH, bool flipV, int imgW, int imgH)
{
	int x = int(pt.x / zoom);
	int y = int(pt.y / zoom);
	if (flipH) x = imgW - 1 - x;
	if (flipV) y = imgH - 1 - y;
	return CPoint(x, y);
}

void CMFCApplicationView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bResizing) {
		m_resizeHit = HitTestResizeHandle(point);
		if (m_resizeHit != RESIZE_NONE) {
			m_bResizing = TRUE;
			m_resizeStartPt = point;
			CMFCApplicationDoc* pDoc = GetDocument();
			m_resizeOrigW = pDoc->m_width;
			m_resizeOrigH = pDoc->m_height;
			m_resizePreviewW = m_resizeOrigW;
			m_resizePreviewH = m_resizeOrigH;
			SetCapture();
			return;
		}
	}

	// === View 좌표 → 이미지 좌표 변환 ===
	CMFCApplicationDoc* pDoc = GetDocument();
	CPoint imgPt = ViewToImage(
		point + GetScrollPosition(), m_zoom, m_bFlipH, m_bFlipV,
		pDoc->m_width, pDoc->m_height
	);

	// === 반드시 이미지 좌표로 캔버스 내부 판정 ===
	if (!IsInCanvas(imgPt)) return;

	// ---- 프리핸드 모드 ---
	if (m_drawType == DRAW_FREEHAND)
	{
		m_bDrawing = TRUE;
		m_tempFreehandPts.clear();
		m_startPoint = imgPt;
		m_tempFreehandPts.push_back(imgPt);
		SetCapture();
		return;
	}

	// ---- 사각형/직선/타원 등 ---
	if (m_drawType != DRAW_NONE)
	{
		m_bDrawing = TRUE;
		m_startPoint = imgPt;
		m_endPoint = imgPt;
		SetCapture();
	}
}



void CMFCApplicationView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bResizing) {
		ResizeHitTest hit = HitTestResizeHandle(point);
		if (hit == RESIZE_CORNER)
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
		else if (hit == RESIZE_RIGHT)
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		else if (hit == RESIZE_BOTTOM)
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
	}
	if (m_bResizing) {
		CPoint delta = point - m_resizeStartPt;
		int newW = m_resizeOrigW;
		int newH = m_resizeOrigH;

		if (m_resizeHit == RESIZE_RIGHT)
			newW = max(50, m_resizeOrigW + delta.x);
		else if (m_resizeHit == RESIZE_BOTTOM)
			newH = max(50, m_resizeOrigH + delta.y);
		else if (m_resizeHit == RESIZE_CORNER) {
			newW = max(50, m_resizeOrigW + delta.x);
			newH = max(50, m_resizeOrigH + delta.y);
		}

		m_resizePreviewW = newW;
		m_resizePreviewH = newH;
		Invalidate(FALSE);
		return;
	}

	if (m_bDrawing)
	{
		// === 좌표 변환 적용 ===
		CMFCApplicationDoc* pDoc = GetDocument();
		CPoint imgPt = ViewToImage(
			point + GetScrollPosition(), m_zoom, m_bFlipH, m_bFlipV,
			pDoc->m_width, pDoc->m_height
		);

		// 도화지 크기 내로 한정
		imgPt.x = max(0, min(imgPt.x, pDoc->m_width - 1));
		imgPt.y = max(0, min(imgPt.y, pDoc->m_height - 1));

		m_endPoint = imgPt;
		Invalidate(TRUE);
	}
	if (m_bDrawing && m_drawType == DRAW_FREEHAND)
	{
		// 드래그 도중 좌표 계속 추가 (이미지 범위 clip)
		CMFCApplicationDoc* pDoc = GetDocument();
		CPoint imgPt = ViewToImage(
			point + GetScrollPosition(), m_zoom, m_bFlipH, m_bFlipV,
			pDoc->m_width, pDoc->m_height
		);
		imgPt.x = max(0, min(imgPt.x, pDoc->m_width - 1));
		imgPt.y = max(0, min(imgPt.y, pDoc->m_height - 1));
		m_tempFreehandPts.push_back(imgPt);
		Invalidate(FALSE);
	}

}

void CMFCApplicationView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bResizing) {
		m_bResizing = FALSE;
		ReleaseCapture();
		int finalW = m_resizePreviewW;
		int finalH = m_resizePreviewH;
		CMFCApplicationDoc* pDoc = GetDocument();
		if (finalW != pDoc->m_width || finalH != pDoc->m_height)
			pDoc->ResizeCanvas(finalW, finalH);
		Invalidate(FALSE);
		return;
	}

	if (m_bDrawing && m_drawType == DRAW_FREEHAND)
	{
		m_bDrawing = FALSE;
		ReleaseCapture();

		if (m_tempFreehandPts.size() >= 2) {
			DrawShape shape;
			shape.type = DRAW_FREEHAND;
			shape.freehandPts = m_tempFreehandPts;
			shape.borderColor = m_curBorderColor;
			shape.borderWidth = m_curBorderWidth;
			shape.fillColor = RGB(255, 255, 255); // 프리핸드는 fill 없음

			// start/end 좌표로도 기록 (첫, 끝점)
			shape.start = m_tempFreehandPts.front();
			shape.end = m_tempFreehandPts.back();

			m_shapes.push_back(shape);
		}
		m_tempFreehandPts.clear();
		Invalidate(FALSE);
		return; // 프리핸드 처리 후 종료!
	}
	else if (m_bDrawing)
	{
		// === 좌표 변환 적용 ===
		CMFCApplicationDoc* pDoc = GetDocument();
		CPoint imgPt = ViewToImage(
			point + GetScrollPosition(), m_zoom, m_bFlipH, m_bFlipV,
			pDoc->m_width, pDoc->m_height
		);
		// ⭐ 드래그 끝점도 반드시 clip
		imgPt.x = max(0, min(imgPt.x, pDoc->m_width - 1));
		imgPt.y = max(0, min(imgPt.y, pDoc->m_height - 1));
		m_endPoint = imgPt;
		m_bDrawing = FALSE;
		ReleaseCapture();

		DrawShape shape;
		shape.type = m_drawType;
		shape.start = m_startPoint;
		shape.end = m_endPoint;
		shape.fillColor = m_curFillColor;
		shape.borderColor = m_curBorderColor;
		shape.borderWidth = m_curBorderWidth;
		m_shapes.push_back(shape);

		Invalidate(FALSE);
	}
}
BOOL CMFCApplicationView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; // 배경 지우기 안함
}


void CMFCApplicationView::OnViewSaveasimage()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("모두 선택"));
	// 파일 다이얼로그 띄움
	CFileDialog dlg(FALSE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("BMP Files (*.bmp)|*.bmp||"));
	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		// 화면 저장 코드 바로 작성!
		CRect rect;
		GetClientRect(&rect);
		CDC memDC;
		CBitmap bitmap;

		CDC* pDC = GetDC();
		memDC.CreateCompatibleDC(pDC);
		bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

		memDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);

		// 저장
		CImage image;
		image.Attach((HBITMAP)bitmap.Detach());
		image.Save(filePath, Gdiplus::ImageFormatBMP);
		image.Detach();

		memDC.SelectObject(pOldBitmap);
		ReleaseDC(pDC);
	}
}

void FlipCImageHorizontal(CImage& img)
{

	int width = img.GetWidth();
	int height = img.GetHeight();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width / 2; ++x)
		{
			COLORREF left = img.GetPixel(x, y);
			COLORREF right = img.GetPixel(width - 1 - x, y);
			img.SetPixel(x, y, right);
			img.SetPixel(width - 1 - x, y, left);
		}
	}
}

void FlipCImageVertical(CImage& img)
{
	if (img.IsNull()) return;
	int width = img.GetWidth();
	int height = img.GetHeight();

	for (int y = 0; y < height / 2; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			COLORREF top = img.GetPixel(x, y);
			COLORREF bottom = img.GetPixel(x, height - 1 - y);
			img.SetPixel(x, y, bottom);
			img.SetPixel(x, height - 1 - y, top);
		}
	}
}

void CMFCApplicationView::OnBnClickedBtnFillColor()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("도형색 선택"));
	CColorDialog dlg(m_curFillColor);
	if (dlg.DoModal() == IDOK)
	{
		m_curFillColor = dlg.GetColor();
	}
}

void CMFCApplicationView::OnBnClickedBtnBorderColor()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("선 색 선택"));
	CColorDialog dlg(m_curBorderColor);
	if (dlg.DoModal() == IDOK)
	{
		m_curBorderColor = dlg.GetColor();
	}
}

void CMFCApplicationView::OnFlipHorizontal()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("좌우 반전"));
	m_bFlipH = !m_bFlipH;   // 좌우 반전 상태 토글
	Invalidate();           // 화면 다시 그리기 요청
}

void CMFCApplicationView::OnFlipVertical()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("상하 반전"));
	m_bFlipV = !m_bFlipV;   // 상하 반전 상태 토글
	Invalidate();           // 화면 다시 그리기 요청
}




CPoint CMFCApplicationView::FlipPoint(const CPoint& pt, int width, int height) const
{
	CPoint out = pt;
	if (m_bFlipH)
		out.x = width - 1 - out.x;
	if (m_bFlipV)
		out.y = height - 1 - out.y;
	return out;
}


//소켓 작업

UINT CMFCApplicationView::SocketThreadProc(LPVOID pParam)
{
	CMFCApplicationView* pView = (CMFCApplicationView*)pParam;

	CSocket serverSocket;
	if (!serverSocket.Create(9999)) {
		AfxMessageBox(_T("소켓 생성 실패"));
		return 1;
	}
	if (!serverSocket.Listen()) {
		AfxMessageBox(_T("소켓 Listen 실패"));
		return 1;
	}

	while (true) {
		CSocket clientSocket;
		if (serverSocket.Accept(clientSocket)) {
			char buf[512] = { 0 };
			int n = clientSocket.Receive(buf, 511);

			if (n > 0) {
				buf[n] = 0;
				CString strCmd(buf);

				if (strCmd.Left(5) == _T("OPEN ")) {
					CString strPath = strCmd.Mid(5).Trim();
					::PostMessage(pView->m_hWnd, WM_USER + 100, 0, (LPARAM)new CString(strPath));
				}
				if (strCmd.Left(6) == _T("SAVEAS")) {
					::PostMessage(pView->m_hWnd, WM_USER + 101, 0, 0);
				}
				if (strCmd.Left(7) == _T("FLIP_H")) {
					::PostMessage(pView->m_hWnd, WM_USER + 102, 0, 0);
				}
				if (strCmd.Left(7) == _T("FLIP_V")) {
					::PostMessage(pView->m_hWnd, WM_USER + 103, 0, 0);
				}
				if (strCmd.Left(9) == _T("CHANNEL_R")) {
					::PostMessage(pView->m_hWnd, WM_USER + 105, 0, 0);
				}
				if (strCmd.Left(9) == _T("CHANNEL_G")) {
					::PostMessage(pView->m_hWnd, WM_USER + 106, 0, 0);
				}
				if (strCmd.Left(9) == _T("CHANNEL_B")) {
					::PostMessage(pView->m_hWnd, WM_USER + 107, 0, 0);
				}
				if (strCmd.Left(10) == _T("DRAW_LINE")) {
					::PostMessage(pView->m_hWnd, WM_USER + 108, 0, 0);
				
				}
				if (strCmd.Left(10) == _T("DRAW_RECT")) {
					::PostMessage(pView->m_hWnd, WM_USER + 109, 0, 0);

				}
				if (strCmd.Left(12) == _T("DRAW_ELLIPSE")) {
					::PostMessage(pView->m_hWnd, WM_USER + 110, 0, 0);
				}
				if (strCmd.Left(12) == _T("SAVE")) {
					::PostMessage(pView->m_hWnd, WM_USER + 111, 0, 0);
				}
				if (strCmd.Left(13) == _T("SET_FILLCOLOR"))
				{
					int r, g, b;
					_stscanf_s(strCmd.Mid(14), _T("%d %d %d"), &r, &g, &b);

					// 메인 윈도우에 메시지로 전달 (스레드-UI 안전하게)
					COLORREF color = RGB(r, g, b);
					::PostMessage(pView->m_hWnd, WM_USER + 112, (WPARAM)color, 0);
				}
				if (strCmd.Left(15) == _T("SET_BORDERCOLOR"))
				{
					int r, g, b;
					_stscanf_s(strCmd.Mid(16), _T("%d %d %d"), &r, &g, &b);
					COLORREF color = RGB(r, g, b);
					::PostMessage(pView->m_hWnd, WM_USER + 200, (WPARAM)color, 0); // 선 색상 변경
				}
				if (strCmd.Left(10) == _T("GRAYSCALE")) {
					::PostMessage(pView->m_hWnd, WM_USER + 201, 0, 0);
				}
				if (strCmd.Left(4) == _T("BLUR")) {
					::PostMessage(pView->m_hWnd, WM_USER + 202, 0, 0);
				}
				if (strCmd.Left(5) == _T("Sobel")) {
					::PostMessage(pView->m_hWnd, WM_USER + 203, 0, 0);
				}
				if (strCmd.Left(5) == _T("Sepia")) {
					::PostMessage(pView->m_hWnd, WM_USER + 204, 0, 0);
				}
				if (strCmd.Left(7) == _T("Defects")) {
					::PostMessage(pView->m_hWnd, WM_USER + 205, 0, 0);
				}
				if (strCmd.Left(5) == _T("Noise")) {
					::PostMessage(pView->m_hWnd, WM_USER + 206, 0, 0);
				}
				if (strCmd.Left(5) == _T("Stain")) {
					::PostMessage(pView->m_hWnd, WM_USER + 207, 0, 0);
				}
			}
			clientSocket.Close();
		}
		Sleep(10);
	}

	return 0;
} 

LRESULT CMFCApplicationView::OnOpenImageFileFromNet(WPARAM, LPARAM lParam)
{
	CString* pPath = (CString*)lParam;

	// 문서 포인터 얻기
	CMFCApplicationDoc* pDoc = GetDocument();

	// 아래처럼 변경!
	if (pDoc->OnOpenDocument(*pPath)) {
		pDoc->SetPathName(*pPath, FALSE); // 경로 및 제목 갱신, bAddToMRU는 필요에 따라 TRUE/FALSE
	}

	delete pPath; // 메모리 해제
	return 0;
}
LRESULT CMFCApplicationView::OnSaveImageFromNet(WPARAM, LPARAM)
{
	CFileDialog dlg(FALSE, _T("bmp"), NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("BMP Files (*.bmp)|*.bmp||"));
	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		GetDocument()->OnSaveDocument(filePath);
	}
	return 0;
}
LRESULT CMFCApplicationView::OnFlipHFromNet(WPARAM, LPARAM)
{
	OnFlipHorizontal();
	Invalidate();
	return 0;
}
LRESULT CMFCApplicationView::OnFlipVFromNet(WPARAM, LPARAM)
{
	OnFlipVertical();
	Invalidate(); // 화면 갱신  
	return 0;
}
LRESULT CMFCApplicationView::OnChannelRFromNet(WPARAM, LPARAM)
{
	GetDocument()->ExtractRGBChannel('R');
	m_selectedChannel = CHANNEL_R;
	Invalidate(); 
	return 0;
}
LRESULT CMFCApplicationView::OnChannelGFromNet(WPARAM, LPARAM)
{
	GetDocument()->ExtractRGBChannel('G');
	m_selectedChannel = CHANNEL_G;
	Invalidate();
	return 0;
}
LRESULT CMFCApplicationView::OnChannelBFromNet(WPARAM, LPARAM)
{
	GetDocument()->ExtractRGBChannel('B');
	m_selectedChannel = CHANNEL_B;
	Invalidate();
	return 0;
}

LRESULT CMFCApplicationView::OnDrawLineFromNet(WPARAM, LPARAM)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("직선 선택"));
	m_drawType = DRAW_LINE;


	Invalidate();
	return 0;
}
LRESULT CMFCApplicationView::OnDrawRectFromNet(WPARAM, LPARAM)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("사각형 선택"));
	m_drawType = DRAW_RECT;


	Invalidate();
	return 0;
}
LRESULT CMFCApplicationView::OnDrawEllipseFromNet(WPARAM, LPARAM)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("원 선택"));

	m_drawType = DRAW_ELLIPSE;
	Invalidate();
	return 0;
}

LRESULT CMFCApplicationView::OnSaveAllFromNet(WPARAM, LPARAM)
{

	CString path = GetDocument()->GetPathName();
	if (!path.IsEmpty()) {
		GetDocument()->OnSaveDocument(path);

		// 로그
		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrm)
			pMainFrm->m_wndOutput.AddLog(_T("기본 저장"));

		Invalidate(); // 필요시 화면 갱신
	}
	else {
		// 만약 경로가 비어있다면(아직 파일로 저장된 적이 없는 경우)
		AfxMessageBox(_T("저장할 경로가 없습니다. (파일을 먼저 열어주세요)"));
	}
	return 0;
}

// 구현
LRESULT CMFCApplicationView::OnSetFillColor(WPARAM wParam, LPARAM)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("도형 색상 선택"));
	m_curFillColor = (COLORREF)wParam;
	// (바로 도형 그릴 때 적용됨)
	return 0;
}

LRESULT CMFCApplicationView::OnSetBorderColor(WPARAM wParam, LPARAM)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("선 색상 선택"));
	m_curBorderColor = (COLORREF)wParam;
	return 0;
}

LRESULT CMFCApplicationView::OnGrayscale(WPARAM wParam, LPARAM)
{
	OnFilterGrayscale();
	return 0;
}
LRESULT CMFCApplicationView::OnBlur(WPARAM wParam, LPARAM)
{
	OnFilterGaussianblur();
	return 0;
}
LRESULT CMFCApplicationView::OnSobel(WPARAM wParam, LPARAM)
{
	OnFilterSobeledge();
	return 0;
}
LRESULT CMFCApplicationView::OnSepia(WPARAM wParam, LPARAM)
{
	OnFilterSepia();
	return 0;
}
LRESULT CMFCApplicationView::OnDefects(WPARAM wParam, LPARAM)
{
	OnDetectDefects();
	return 0;
}
LRESULT CMFCApplicationView::OnNoise(WPARAM wParam, LPARAM)
{
	OnCheckNoise();
	return 0;
}
LRESULT CMFCApplicationView::OnStain(WPARAM wParam, LPARAM)
{
	OnDetectStain();
	return 0;
}

void CMFCApplicationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// 소켓 서버 스레드 (기존 코드 그대로)
	if (m_pThread == nullptr) {
		m_pThread = AfxBeginThread(SocketThreadProc, this);
	}
	CMFCApplicationDoc* pDoc = GetDocument();
	if (pDoc && pDoc->m_width > 0 && pDoc->m_height > 0) {
		SetScrollSizes(MM_TEXT, CSize(
			int(max(pDoc->m_width * m_zoom, 800)),
			int(max(pDoc->m_height * m_zoom, 600))
		));
	}
	else {
		SetScrollSizes(MM_TEXT, CSize(800, 600));
	}
}


// 도화지의 오른쪽/아래/모서리 핸들 영역 체크
CMFCApplicationView::ResizeHitTest CMFCApplicationView::HitTestResizeHandle(CPoint pt)
{
	CMFCApplicationDoc* pDoc = GetDocument();
	int canvasW = pDoc->m_width;
	int canvasH = pDoc->m_height;
	const int HANDLE_SIZE = 10;

	// pt는 "뷰 좌표"임 (스크롤 오프셋 이미 포함된 상태라면 아래 한 줄 생략 가능)
	pt += GetScrollPosition();

	// === 반드시 "이미지 좌표계"로 변환 ===
	CPoint imgPt = ViewToImage(pt, m_zoom, m_bFlipH, m_bFlipV, canvasW, canvasH);

	// 이제 imgPt와 도화지 끝(캔버스 크기) 비교
	if (abs(imgPt.x - canvasW) <= HANDLE_SIZE && abs(imgPt.y - canvasH) <= HANDLE_SIZE)
		return RESIZE_CORNER;
	if (abs(imgPt.x - canvasW) <= HANDLE_SIZE && imgPt.y < canvasH)
		return RESIZE_RIGHT;
	if (abs(imgPt.y - canvasH) <= HANDLE_SIZE && imgPt.x < canvasW)
		return RESIZE_BOTTOM;
	return RESIZE_NONE;
}


bool CMFCApplicationView::IsInCanvas(CPoint pt)
{
	CMFCApplicationDoc* pDoc = GetDocument();
	int x = pt.x + GetScrollPosition().x;
	int y = pt.y + GetScrollPosition().y;
	return (x >= 0 && x < pDoc->m_width && y >= 0 && y < pDoc->m_height);
}

void CMFCApplicationView::DrawAllShapesToDC(CDC* pDC)
{
	int nW = GetDocument()->m_width;
	int nH = GetDocument()->m_height;

	auto FlipPoint = [this, nW, nH](const CPoint& pt) -> CPoint {
		CPoint res = pt;
		if (m_bFlipH) res.x = nW - 1 - res.x;
		if (m_bFlipV) res.y = nH - 1 - res.y;
		return res;
		};

	for (const auto& shape : m_shapes)
	{
		CPoint pt1 = FlipPoint(shape.start);
		CPoint pt2 = FlipPoint(shape.end);

		CPen pen(PS_SOLID, shape.borderWidth, shape.borderColor);
		CPen* pOldPen = pDC->SelectObject(&pen);
		CBrush brush(shape.fillColor);
		CBrush* pOldBrush = pDC->SelectObject(&brush);

		switch (shape.type)
		{
		case DRAW_LINE:    pDC->MoveTo(pt1); pDC->LineTo(pt2); break;
		case DRAW_RECT:    pDC->Rectangle(CRect(pt1, pt2)); break;
		case DRAW_ELLIPSE: pDC->Ellipse(CRect(pt1, pt2)); break;
		}
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
}

void CMFCApplicationView::OnFileSaveAs()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CFileDialog dlg(FALSE, _T("bmp"), NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("BMP Files (*.bmp)|*.bmp||"));
	if (dlg.DoModal() != IDOK)
		return;

	CString filePath = dlg.GetPathName();

	// Document 포인터 얻어서 직접 저장 호출
	CMFCApplicationDoc* pDoc = GetDocument();
	if (pDoc)
		pDoc->OnSaveDocument(filePath);
}

void CMFCApplicationView::OnFilterGrayscale()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("그레이스케일(흑백) 필터 적용"));

	GetDocument()->ApplyGrayscale();
}

void CMFCApplicationView::OnFilterGaussianblur()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("가우시안 블러 필터 적용"));

	GetDocument()->ApplyGaussianBlur();
}

void CMFCApplicationView::OnFilterSobeledge()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("엣지 검출(소벨) 필터 적용"));

	GetDocument()->ApplySobelEdge();
}

void CMFCApplicationView::OnFilterSepia()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("세피아톤 필터 적용"));

	GetDocument()->ApplySepia();
}

void CMFCApplicationView::OnEditUndo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GetDocument()->Undo();
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
		pMainFrm->m_wndOutput.AddLog(_T("Undo(되돌리기) 실행"));
}

BOOL CMFCApplicationView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// Ctrl 키가 눌렸을 때만 동작
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		double prevZoom = m_zoom;
		if (zDelta > 0)      m_zoom *= 1.1; // 확대
		else if (zDelta < 0) m_zoom /= 1.1; // 축소

		if (m_zoom < 0.1) m_zoom = 0.1;
		if (m_zoom > 10.0) m_zoom = 10.0;

		// 도화지(이미지) 크기에 배율 적용해서 스크롤 크기 설정
		CMFCApplicationDoc* pDoc = GetDocument();
		int w = int(pDoc->m_width * m_zoom);
		int h = int(pDoc->m_height * m_zoom);
		SetScrollSizes(MM_TEXT, CSize(w, h));

		Invalidate(FALSE); // 화면 다시 그리기
		return TRUE;
	}
	// Ctrl 없이 휠 돌리면 원래 스크롤 기능
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMFCApplicationView::OnDetectDefects()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	GetDocument()->DetectDefects();
	Invalidate(FALSE); // 갱신
}

void CMFCApplicationView::OnCheckNoise()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMFCApplicationDoc* pDoc = GetDocument();
	if (!pDoc) return;

	// Gray 변환 (최근 이미지 기준, 기존 방식 재활용)
	int w = pDoc->m_width, h = pDoc->m_height;
	if (!pDoc->m_pImage) return;
	std::vector<BYTE> gray(w * h);
	for (int i = 0; i < w * h; ++i) {
		BYTE b = pDoc->m_pImage[i * 3 + 0];
		BYTE g = pDoc->m_pImage[i * 3 + 1];
		BYTE r = pDoc->m_pImage[i * 3 + 2];
		gray[i] = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b + 0.5);
	}

	// 표준편차(노이즈) 계산
	double sum = 0, sqsum = 0;
	int N = (int)gray.size();
	for (int i = 0; i < N; ++i) {
		sum += gray[i];
		sqsum += double(gray[i]) * gray[i];
	}
	double mean = sum / N;
	double var = (sqsum / N) - (mean * mean);
	double stddev = sqrt(var);

	// 노이즈 판정 (예: 15.0 기준)
	bool isNoiseOK = (stddev < 15.0);

	// 로그로 결과 남기기
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm) {
		pMainFrm->m_wndOutput.AddLog(_T("== Noise 검출 =="));

		CString msg;
		msg.Format(_T("StdDev: %.2f - %s"), stddev, isNoiseOK ? _T("PASS") : _T("FAIL"));
		pMainFrm->m_wndOutput.AddLog(msg);
	}

	// (화면에 별도 PASS/FAIL 표시 원하면, 멤버변수로 상태 저장 후 Invalidate 호출)
	pDoc->m_stddev = stddev; // Doc에 저장해두면 OnDraw에서 활용 가능
	Invalidate(FALSE);       // 화면 갱신
}

void CMFCApplicationView::OnDetectStain()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMFCApplicationDoc* pDoc = GetDocument();
	if (pDoc) {
		pDoc->DetectStainRegions(); // 실제 검출
		Invalidate(FALSE); // 뷰 갱신(박스 표시)
	}
}
void CMFCApplicationView::RemoveShapesOutsideCanvas(int canvasW, int canvasH)
{
	auto isInCanvas = [canvasW, canvasH](const DrawShape& s) {
		return (s.start.x >= 0 && s.start.x < canvasW && s.start.y >= 0 && s.start.y < canvasH &&
			s.end.x >= 0 && s.end.x < canvasW && s.end.y >= 0 && s.end.y < canvasH);
		};
	m_shapes.erase(
		std::remove_if(m_shapes.begin(), m_shapes.end(), [=](const DrawShape& s) {
			return !isInCanvas(s);
			}),
		m_shapes.end()
	);
}
