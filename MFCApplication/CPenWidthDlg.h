#pragma once
#include <afxcmn.h>

class CPenWidthDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CPenWidthDlg)

public:
    CPenWidthDlg(CWnd* pParent = nullptr);

    int m_penWidth;  // 펜 굵기 값 (호출자와 주고받음)

    enum { IDD = IDD_PEN_WIDTH_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    DECLARE_MESSAGE_MAP()

    CSliderCtrl m_slider;
    CStatic* m_staticWidth; // (선택) 슬라이더 값 실시간 표시
};
