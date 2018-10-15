#pragma once
#include<opencv2/core/core.hpp>

using namespace cv;
class CSudokuSolver
{
public:
	CSudokuSolver();
	~CSudokuSolver();
	bool SolveRecursive(Mat& sudokuValues);
	bool FindUnassignedLocation(Mat& sudokuValues, int &row, int &col);
	bool UsedInRow(Mat& sudokuValues, int row, int num);
	bool UsedInCol(Mat& sudokuValues, int col, int num);
	bool UsedInBox(Mat& sudokuValues, int boxStartRow, int boxStartCol, int num);
	bool isSafe(Mat& sudokuValues,int row, int col, int num);
};
