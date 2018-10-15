#pragma once
#include"ContourWithData.h"
class CSudokuGrabberAndSolverDlg;
using namespace std;
using namespace cv;
class SudokuReader
{
	enum {
		RESIZED_IMAGE_WIDTH = 20,
		RESIZED_IMAGE_HEIGHT = 30
	};
	CSudokuGrabberAndSolverDlg* m_pParent;
	void drawLine(cv::Vec2f line, cv::Mat &img, cv::Scalar rgb);
	void mergeRelatedLines(std::vector<cv::Vec2f> *lines, cv::Mat &img);
public:
	SudokuReader(CSudokuGrabberAndSolverDlg* pParent);
	~SudokuReader();
	void ReadSudoku(const char* strFilename, Mat& sudokuValues);
};

