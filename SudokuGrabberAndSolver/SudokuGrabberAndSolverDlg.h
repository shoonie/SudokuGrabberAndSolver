#pragma once
#include "ContourWithData.h"

class CSudokuGrabberAndSolverDlg : public CDialog
{
// Construction
public:
	CSudokuGrabberAndSolverDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUDOKUGRABBERANDSOLVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CImage* m_pImg;
	CEdit m_LogBox;
	CStatic m_Panel;
public:
	afx_msg void OnBnClickedTraining();
	afx_msg void OnBnClickedTesttraining();
	afx_msg void OnBnClickedReadsudoku();
	afx_msg void OnBnClickedSolvesudokubacktracking();
	afx_msg void OnBnClickedExit();
	void LogStatus(char*  text, ...);
	void PrintCurrent();
	unsigned char m_nValues[9][9];
	void	DrawOnPanel(int OriginXsize, int OriginYSize, cv::Mat dataGraphic);
};