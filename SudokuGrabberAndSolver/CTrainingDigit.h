#pragma once
#include"ContourWithData.h"
class CSudokuGrabberAndSolverDlg;
class CTrainingDigit
{
	enum {
		MIN_CONTOUR_AREA = 30
	};
	enum {
		RESIZED_IMAGE_WIDTH = 20, 
		RESIZED_IMAGE_HEIGHT = 30
	};
	CSudokuGrabberAndSolverDlg* m_pParent;
public:
	CTrainingDigit(CSudokuGrabberAndSolverDlg* pParent);
	~CTrainingDigit();
	void TrainingDigit(const char*  strFilename);
	void TestTraining(const char*  strFilename);
};