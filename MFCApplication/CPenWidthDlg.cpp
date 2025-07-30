// CPenWidthDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCApplication.h"
#include "afxdialogex.h"
#include "CPenWidthDlg.h"


// CPenWidthDlg 대화 상자

IMPLEMENT_DYNAMIC(CPenWidthDlg, CDialogEx)

CPenWidthDlg::CPenWidthDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_PEN_WIDTH_DLG, pParent)
    , m_penWidth(1)
{
}

void CPenWidthDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLIDER_WIDTH, m_slider);
    // (선택) Static 값 표시
}

BEGIN_MESSAGE_MAP(CPenWidthDlg, CDialogEx)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CPenWidthDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_slider.SetRange(1, 50); // 범위 1~50
    m_slider.SetTicFreq(1);
    m_slider.SetPos(m_penWidth);

    // (선택) Static 텍스트에 현재 값 출력
    SetDlgItemInt(IDC_STATIC_WIDTH, m_penWidth);

    return TRUE;
}

void CPenWidthDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (pScrollBar == (CScrollBar*)&m_slider)
    {
        m_penWidth = m_slider.GetPos();
        // (선택) Static 텍스트에 현재 값 출력
        SetDlgItemInt(IDC_STATIC_WIDTH, m_penWidth);
    }
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}