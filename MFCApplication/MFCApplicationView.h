// MFCApplicationView.h: CMFCApplicationView 클래스의 인터페이스
//

#pragma once
#include <vector>  // 추가
#include <afxsock.h>

class CMFCApplicationView : public CView
{
protected: // serialization에서만 만들어집니다.
    CMFCApplicationView() noexcept;
    DECLARE_DYNCREATE(CMFCApplicationView)


    CSocket m_serverSocket; // 서버 소켓
    CSocket m_clientSocket; // 클라이언트 소켓
    CWinThread* m_pThread = nullptr; // 소켓 쓰레드
    static UINT SocketThreadProc(LPVOID pParam);
    virtual void OnInitialUpdate();
    // 채널 타입


    // 그림판 도형 타입
    enum DrawType { DRAW_NONE, DRAW_LINE, DRAW_RECT, DRAW_ELLIPSE };

    //도형 정보 저장용 구조체
    struct DrawShape {
        DrawType type;
        CPoint start;
        CPoint end;
        COLORREF fillColor = RGB(255, 255, 255);    // 채우기 색 
        COLORREF borderColor = RGB(0, 0, 0);        // 외곽선 색 
        int borderWidth = 1;                        // 외곽선 두께
    };

    COLORREF m_curFillColor = RGB(255, 255, 255);
    COLORREF m_curBorderColor = RGB(0, 0, 0);
    int      m_curBorderWidth = 1;
    DrawType m_drawType = DRAW_NONE;         // 현재 선택된 도형 종류
    BOOL     m_bDrawing = FALSE;             // 드래그 중 여부
    CPoint   m_startPoint;                   // 드래그 시작점
    CPoint   m_endPoint;                     // 드래그 끝점
    std::vector<DrawShape> m_shapes;         // 그려진 도형 리스트


    // 특성입니다.
public:
    CMFCApplicationDoc* GetDocument() const;
    enum ChannelType { CHANNEL_ORG, CHANNEL_R, CHANNEL_G, CHANNEL_B };
    ChannelType m_selectedChannel = CHANNEL_ORG;



    // 작업입니다.
public:

    // 재정의입니다.
public:
    virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

    // 구현입니다.
public:
    virtual ~CMFCApplicationView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    // 생성된 메시지 맵 함수
protected:
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnViewChannelR();
    afx_msg void OnViewChannelG();
    afx_msg void OnViewChannelB();

    // 그림판 기능 함수 선언
    afx_msg void OnDrawLine();
    afx_msg void OnDrawRect();
    afx_msg void OnDrawEllipse();

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnViewSaveasimage();

    afx_msg void OnFlipHorizontal();
    afx_msg void OnFlipVertical();

    //소켓
    afx_msg LRESULT OnOpenImageFileFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnSaveImageFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnFlipHFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnFlipVFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnChannelRFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnChannelGFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnChannelBFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnDrawLineFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnDrawRectFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnDrawEllipseFromNet(WPARAM, LPARAM);
    afx_msg LRESULT OnSaveAllFromNet(WPARAM, LPARAM);
    afx_msg void OnBnClickedBtnFillColor();
    afx_msg void OnBnClickedBtnBorderColor();

};

#ifndef _DEBUG  // MFCApplicationView.cpp의 디버그 버전
inline CMFCApplicationDoc* CMFCApplicationView::GetDocument() const
{
    return reinterpret_cast<CMFCApplicationDoc*>(m_pDocument);
}
#endif
