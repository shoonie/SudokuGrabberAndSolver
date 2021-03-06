
// SudokuGrabberAndSolverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SudokuGrabberAndSolver.h"
#include "SudokuGrabberAndSolverDlg.h"
#include "afxdialogex.h"
#include "CSudokuSolver.h"
#include "CTrainingDigit.h"
#include "SudokuReader.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CSudokuGrabberAndSolverDlg dialog

CSudokuGrabberAndSolverDlg::CSudokuGrabberAndSolverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SUDOKUGRABBERANDSOLVER_DIALOG, pParent)
	,m_pImg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			m_nValues[i][j] = 0;
}

void CSudokuGrabberAndSolverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGEDIT, m_LogBox);
	DDX_Control(pDX, IDC_DISPLAYWINDOW, m_Panel);
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			DDX_Text(pDX, IDC_EDIT11 + j + i * 10, m_nValues[i][j]);
			DDV_MinMaxInt(pDX, m_nValues[i][j], 0, 9);
		}
	}
}

BEGIN_MESSAGE_MAP(CSudokuGrabberAndSolverDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDTRAINING, &CSudokuGrabberAndSolverDlg::OnBnClickedTraining)
	ON_BN_CLICKED(IDTESTTRAINING, &CSudokuGrabberAndSolverDlg::OnBnClickedTesttraining)
	ON_BN_CLICKED(IDREADSUDOKU, &CSudokuGrabberAndSolverDlg::OnBnClickedReadsudoku)
	ON_BN_CLICKED(IDEXIT, &CSudokuGrabberAndSolverDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDSOLVESUDOKUBACKTRACKING, &CSudokuGrabberAndSolverDlg::OnBnClickedSolvesudokubacktracking)
END_MESSAGE_MAP()


// CSudokuGrabberAndSolverDlg message handlers

BOOL CSudokuGrabberAndSolverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_Panel.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSudokuGrabberAndSolverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSudokuGrabberAndSolverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSudokuGrabberAndSolverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSudokuGrabberAndSolverDlg::LogStatus(char*  text, ...)
{

	char buf[512] = { 0, };
	va_list ap;

	va_start(ap, text);

	//vswprintf(buf + wcslen(buf), fmt, ap);
	vsprintf_s(buf, 512, text, ap);
	va_end(ap);



	const size_t cSize = strlen(buf) + 1;
	wchar_t* wc = new wchar_t[cSize];
	size_t outSize;

	mbstowcs_s(&outSize, wc, cSize, buf, cSize - 1);

	int nLength = m_LogBox.GetWindowTextLength();
	m_LogBox.SetSel(nLength, nLength);
	m_LogBox.ReplaceSel(wc);
	delete[]wc;
}

void	CSudokuGrabberAndSolverDlg::DrawOnPanel(int OriginXsize, int OriginYSize, cv::Mat dataGraphic) 
{
	OriginXsize = OriginXsize / 4 * 4;

	int xSize = OriginXsize < 500 ? OriginXsize : 500;
	int ySize = OriginYSize < 350 ? OriginYSize : 350;

	m_Panel.MoveWindow(10, 10, xSize, ySize);
	m_Panel.ShowWindow(SW_SHOW);
	if (m_pImg)
	{
		m_pImg->ReleaseDC();
		delete m_pImg;
		m_pImg = NULL;
	}
	m_pImg = new CImage;
	m_pImg->Create(xSize, ySize, 24);
	cv::Mat displayTraining;			// for display on the dlg.
	cv::resize(dataGraphic, displayTraining, cv::Size(xSize, ySize));
	cv::flip(displayTraining, displayTraining, 0);
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 24;
	bitInfo.bmiHeader.biWidth = xSize;
	bitInfo.bmiHeader.biHeight = ySize;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant =
		bitInfo.bmiHeader.biClrUsed =
		bitInfo.bmiHeader.biSizeImage =
		bitInfo.bmiHeader.biXPelsPerMeter =
		bitInfo.bmiHeader.biYPelsPerMeter = 0;

	StretchDIBits(m_pImg->GetDC(), 0, 0,
		xSize, ySize, 0, 0,
		xSize, ySize,
		displayTraining.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);

	m_pImg->BitBlt(::GetDC(m_Panel.m_hWnd), 0, 0);
	displayTraining.release();
}

void CSudokuGrabberAndSolverDlg::OnBnClickedTraining()
{
	char strFilter[] = { "All Files (*.*)|*.*|" };
	CFileDialog FileDlg(TRUE, CString("Open Graphic file"), NULL, 0, CString(strFilter));
	CString filename;
	if (FileDlg.DoModal() == IDOK)
	{
		filename = FileDlg.GetPathName(); // return full path and filename
	}
	else
		return;
	USES_CONVERSION;
	const char* strFilename = T2A((LPCTSTR)filename);

	CTrainingDigit trainingDigit(this);
	trainingDigit.TrainingDigit(strFilename);
	return;
}

void CSudokuGrabberAndSolverDlg::OnBnClickedTesttraining()
{
	char strFilter[] = { "All Files (*.*)|*.*|" };
	CFileDialog FileDlg(TRUE, CString("Open Test Graphic file"), NULL, 0, CString(strFilter));
	CString filename;
	if (FileDlg.DoModal() == IDOK)
	{
		filename = FileDlg.GetPathName(); // return full path and filename
	}
	else
		return;
	USES_CONVERSION;
	const char* strFilename = T2A((LPCTSTR)filename);

	CTrainingDigit trainingDigit(this);
	trainingDigit.TestTraining(strFilename);
	return;
}

void CSudokuGrabberAndSolverDlg::OnBnClickedReadsudoku()
{
	char strFilter[] = { "All Files (*.*)|*.*|" };
	CFileDialog FileDlg(TRUE, CString("Open Sudoku Graphic file"), NULL, 0, CString(strFilter));
	CString filename;
	if (FileDlg.DoModal() == IDOK)
	{
		filename = FileDlg.GetPathName(); // return full path and filename
	}
	else
		return;
	LogStatus("\n New Sudoku \n");
	USES_CONVERSION;
	const char* strFilename = T2A((LPCTSTR)filename);
	SudokuReader reader(this);
	Mat sudokuValue = Mat(9, 9, CV_8U, m_nValues);
	reader.ReadSudoku(strFilename, sudokuValue);
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			m_nValues[i][j] = sudokuValue.at<unsigned char>(i, j);
	UpdateData(FALSE);
}

void CSudokuGrabberAndSolverDlg::OnBnClickedExit()
{
	OnOK();
}

void CSudokuGrabberAndSolverDlg::OnBnClickedSolvesudokubacktracking()
{
	UpdateData(TRUE);
	Mat sudokuValue = Mat(9, 9, CV_8U, m_nValues);
	CSudokuSolver aSolver;
	DWORD nStartTIme = GetTickCount();
	aSolver.SolveRecursive(sudokuValue);

	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			m_nValues[i][j] = sudokuValue.at<unsigned char>(i, j);
	DWORD nEndTime= GetTickCount() - nStartTIme;
	UpdateData(FALSE);
	PrintCurrent();
	LogStatus("\n\n Done. %d ms \n", nEndTime);
}

void CSudokuGrabberAndSolverDlg::PrintCurrent()
{
	LogStatus("\n");
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			LogStatus("%d ", m_nValues[i][j]);
		}
		LogStatus("\n");
	}
}