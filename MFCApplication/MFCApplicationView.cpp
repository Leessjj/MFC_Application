
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplicationView

IMPLEMENT_DYNCREATE(CMFCApplicationView, CView)

BEGIN_MESSAGE_MAP(CMFCApplicationView, CView)
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

// CMFCApplicationView 그리기
/*
void CMFCApplicationView::OnDraw(CDC* pDC)
{

	CMFCApplicationDoc* pDoc = GetDocument();
	switch (m_selectedChannel) {
	case CHANNEL_R:
		if (!pDoc->m_channelR.IsNull())
			pDoc->m_channelR.Draw(pDC->GetSafeHdc(), 0, 0);
		break;
	case CHANNEL_G:
		if (!pDoc->m_channelG.IsNull())
			pDoc->m_channelG.Draw(pDC->GetSafeHdc(), 0, 0);
		break;
	case CHANNEL_B:
		if (!pDoc->m_channelB.IsNull())
			pDoc->m_channelB.Draw(pDC->GetSafeHdc(), 0, 0);
		break;
	default:
		if (!pDoc->m_image.IsNull())
			pDoc->m_image.Draw(pDC->GetSafeHdc(), 0, 0);
	}
	// 2. 그려진 도형 반복 출력
	// 저장된 도형 그리기
	 // 저장된 도형 그리기
	for (const auto& shape : m_shapes)
	{
		// 브러시/펜 생성
		CBrush brush(shape.fillColor);                  // 채움색
		CPen pen(PS_SOLID, shape.borderWidth, shape.borderColor); // 외곽선
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		CPen* pOldPen = pDC->SelectObject(&pen);

		switch (shape.type)
		{
		case DRAW_LINE:
			pDC->MoveTo(shape.start);
			pDC->LineTo(shape.end);
			break;
		case DRAW_RECT:
			pDC->Rectangle(CRect(shape.start, shape.end));    // 채우기/외곽선 동시적용!
			break;
		case DRAW_ELLIPSE:
			pDC->Ellipse(CRect(shape.start, shape.end));
			break;
		}
		// 원상복귀
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}

	// --- 미리보기(현재 그리고 있는 도형) ---
	if (m_bDrawing && m_drawType != DRAW_NONE)
	{
		// 예: m_curFillColor, m_curBorderColor, m_curBorderWidth 등
		CBrush brush(m_curFillColor);
		CPen pen(PS_SOLID, m_curBorderWidth, m_curBorderColor);
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		CPen* pOldPen = pDC->SelectObject(&pen);

		switch (m_drawType)
		{
		case DRAW_LINE:
			pDC->MoveTo(m_startPoint);
			pDC->LineTo(m_endPoint);
			break;
		case DRAW_RECT:
			pDC->Rectangle(CRect(m_startPoint, m_endPoint));
			break;
		case DRAW_ELLIPSE:
			pDC->Ellipse(CRect(m_startPoint, m_endPoint));
			break;
		}
		// Pen/Brush 원복!
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
}*/
void CMFCApplicationView::OnDraw(CDC* pDC)
{
	CMFCApplicationDoc* pDoc = GetDocument();
	if (!pDoc) return;

	// 1. 클라이언트 크기 (도형 반전용)
	CRect clientRect;
	GetClientRect(&clientRect);
	int viewW = clientRect.Width();
	int viewH = clientRect.Height();

	// 2. 현재 선택된 이미지 채널
	CImage* pImg = nullptr;
	switch (m_selectedChannel) {
	case CHANNEL_R: pImg = &pDoc->m_channelR; break;
	case CHANNEL_G: pImg = &pDoc->m_channelG; break;
	case CHANNEL_B: pImg = &pDoc->m_channelB; break;
	default:        pImg = &pDoc->m_image;    break;
	}

	// 3. 이미지 그리기 (반전 포함)
	if (pImg && !pImg->IsNull()) {
		int nW = pImg->GetWidth();
		int nH = pImg->GetHeight();
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC, nW, nH);
		CBitmap* pOldBmp = memDC.SelectObject(&bmp);
		pImg->Draw(memDC.GetSafeHdc(), 0, 0);

		int destX = 0, destY = 0;
		int destW = nW, destH = nH;
		int srcX = 0, srcY = 0;
		int srcW = nW, srcH = nH;

		// [뷰 크기]에 맞게 반전 위치 계산
		if (m_bFlipH) destX = viewW - nW;
		if (m_bFlipV) destY = viewH - nH;

		if (m_bFlipH && m_bFlipV)
			pDC->StretchBlt(destX + nW - 1, destY + nH - 1, -nW, -nH, &memDC, 0, 0, nW, nH, SRCCOPY);
		else if (m_bFlipH)
			pDC->StretchBlt(destX + nW - 1, destY, -nW, nH, &memDC, 0, 0, nW, nH, SRCCOPY);
		else if (m_bFlipV)
			pDC->StretchBlt(destX, destY + nH - 1, nW, -nH, &memDC, 0, 0, nW, nH, SRCCOPY);
		else
			pDC->BitBlt(0, 0, nW, nH, &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBmp);
	}


	// 4. 도형 좌표 변환 함수 (뷰 전체 기준)
	auto FlipPoint = [this, viewW, viewH](const CPoint& pt) -> CPoint {
		CPoint res = pt;
		if (m_bFlipH) res.x = viewW - 1 - res.x;
		if (m_bFlipV) res.y = viewH - 1 - res.y;
		return res;
		};

	// 5. 저장된 도형 그리기 (반전 적용)
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
		case DRAW_LINE:
			pDC->MoveTo(pt1); pDC->LineTo(pt2); break;
		case DRAW_RECT:
			pDC->Rectangle(CRect(pt1, pt2)); break;
		case DRAW_ELLIPSE:
			pDC->Ellipse(CRect(pt1, pt2)); break;
		}
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}

	// 6. 미리보기 도형 (반전 적용)
	if (m_bDrawing && m_drawType != DRAW_NONE)
	{
		CPoint pt1 = FlipPoint(m_startPoint);
		CPoint pt2 = FlipPoint(m_endPoint);

		CPen pen(PS_SOLID, m_curBorderWidth, m_curBorderColor);
		CPen* pOldPen = pDC->SelectObject(&pen);
		CBrush brush(m_curFillColor);
		CBrush* pOldBrush = pDC->SelectObject(&brush);

		switch (m_drawType)
		{
		case DRAW_LINE:
			pDC->MoveTo(pt1); pDC->LineTo(pt2); break;
		case DRAW_RECT:
			pDC->Rectangle(CRect(pt1, pt2)); break;
		case DRAW_ELLIPSE:
			pDC->Ellipse(CRect(pt1, pt2)); break;
		}
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
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
	m_drawType = DRAW_LINE;
}

void CMFCApplicationView::OnDrawRect()
{
	m_drawType = DRAW_RECT;
}

void CMFCApplicationView::OnDrawEllipse()
{
	m_drawType = DRAW_ELLIPSE;
}


void CMFCApplicationView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_drawType != DRAW_NONE)
	{
		m_bDrawing = TRUE;
		m_startPoint = point;
		m_endPoint = point;
		SetCapture();  // 마우스 캡처
	}
}

void CMFCApplicationView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDrawing)
	{
		m_endPoint = point;
		Invalidate(TRUE);  // 화면 다시 그리기
	}
}

void CMFCApplicationView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDrawing) {
		m_endPoint = point;
		m_bDrawing = FALSE;
		ReleaseCapture();

		// 도형 정보를 리스트에 저장
		DrawShape shape;
		shape.type = m_drawType;
		shape.start = m_startPoint;
		shape.end = m_endPoint;
		shape.fillColor = m_curFillColor;       // <- 이 줄이 반드시 필요!
		shape.borderColor = m_curBorderColor;
		shape.borderWidth = m_curBorderWidth;
		m_shapes.push_back(shape);

		Invalidate(FALSE); // 최종 도형 그리기
	}
}

void CMFCApplicationView::OnViewSaveasimage()
{
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
	if (img.IsNull()) return;
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

//void CMFCApplicationView::OnFlipHorizontal()
//{
//	CMFCApplicationDoc* pDoc = GetDocument();
//	switch (m_selectedChannel) {
//	case CHANNEL_R:
//		FlipCImageHorizontal(pDoc->m_channelR);
//		break;
//	case CHANNEL_G:
//		FlipCImageHorizontal(pDoc->m_channelG);
//		break;
//	case CHANNEL_B:
//		FlipCImageHorizontal(pDoc->m_channelB);
//		break;
//	default:
//		pDoc->OnImageFlipHorizontal();
//	}
//	Invalidate();
//}

//void CMFCApplicationView::OnFlipVertical()
//{
//	CMFCApplicationDoc* pDoc = GetDocument();
//	switch (m_selectedChannel) {
//	case CHANNEL_R:
//		FlipCImageVertical(pDoc->m_channelR);
//		break;
//	case CHANNEL_G:
//		FlipCImageVertical(pDoc->m_channelG);
//		break;
//	case CHANNEL_B:
//		FlipCImageVertical(pDoc->m_channelB);
//		break;
//	default:
//		pDoc->OnImageFlipVertical();
//	}
//	Invalidate();
//}

void CMFCApplicationView::OnBnClickedBtnFillColor()
{
	CColorDialog dlg(m_curFillColor);
	if (dlg.DoModal() == IDOK)
	{
		m_curFillColor = dlg.GetColor();
	}
}

void CMFCApplicationView::OnBnClickedBtnBorderColor()
{
	CColorDialog dlg(m_curBorderColor);
	if (dlg.DoModal() == IDOK)
	{
		m_curBorderColor = dlg.GetColor();
	}
}

void CMFCApplicationView::OnFlipHorizontal()
{
	m_bFlipH = !m_bFlipH;   // 좌우 반전 상태 토글
	Invalidate();           // 화면 다시 그리기 요청
}

void CMFCApplicationView::OnFlipVertical()
{
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
					AfxMessageBox(strCmd);
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
				if (strCmd.Left(12) == _T("SAVE_ALL")) {
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

	GetDocument()->OnOpenDocument(*pPath);
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
	CMFCApplicationDoc* pDoc = GetDocument();
	switch (m_selectedChannel) {
	case CHANNEL_R:
		FlipCImageHorizontal(pDoc->m_channelR);
		break;
	case CHANNEL_G:
		FlipCImageHorizontal(pDoc->m_channelG);
		break;
	case CHANNEL_B:
		FlipCImageHorizontal(pDoc->m_channelB);
		break;
	default:
		pDoc->OnImageFlipHorizontal();
	}
	Invalidate();
	return 0;
}
LRESULT CMFCApplicationView::OnFlipVFromNet(WPARAM, LPARAM)
{
	CMFCApplicationDoc* pDoc = GetDocument();
	switch (m_selectedChannel) {
	case CHANNEL_R:
		FlipCImageVertical(pDoc->m_channelR);
		break;
	case CHANNEL_G:
		FlipCImageVertical(pDoc->m_channelG);
		break;
	case CHANNEL_B:
		FlipCImageVertical(pDoc->m_channelB);
		break;
	default:
		pDoc->OnImageFlipVertical();
	}
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
	m_drawType = DRAW_LINE;


	Invalidate();
	return 0;
}
LRESULT CMFCApplicationView::OnDrawRectFromNet(WPARAM, LPARAM)
{
	m_drawType = DRAW_RECT;


	Invalidate();
	return 0;
}
LRESULT CMFCApplicationView::OnDrawEllipseFromNet(WPARAM, LPARAM)
{
	m_drawType = DRAW_ELLIPSE;


	Invalidate();
	return 0;
}

LRESULT CMFCApplicationView::OnSaveAllFromNet(WPARAM, LPARAM)
{
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
	Invalidate();
	return 0;
}

// 구현
LRESULT CMFCApplicationView::OnSetFillColor(WPARAM wParam, LPARAM)
{
	m_curFillColor = (COLORREF)wParam;
	// (바로 도형 그릴 때 적용됨)
	return 0;
}

LRESULT CMFCApplicationView::OnSetBorderColor(WPARAM wParam, LPARAM)
{
	m_curBorderColor = (COLORREF)wParam;
	return 0;
}


void CMFCApplicationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// 소켓 서버 스레드
	if (m_pThread == nullptr) {
		m_pThread = AfxBeginThread(SocketThreadProc, this);
	}
}

