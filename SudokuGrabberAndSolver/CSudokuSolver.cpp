#include "stdafx.h"
#include "CSudokuSolver.h"
CSudokuSolver::CSudokuSolver()
{
}

CSudokuSolver::~CSudokuSolver()
{
}

bool CSudokuSolver::SolveRecursive(Mat& sudokuValues)
{
	int row, col;
	if (!FindUnassignedLocation(sudokuValues,row, col))
		return true;
	for (int num = 1; num <= 9; ++num)
	{
		if (isSafe(sudokuValues,row, col, num))
		{
			sudokuValues.at<unsigned char>(row, col) = num;
			if (SolveRecursive(sudokuValues))
				return true;
			sudokuValues.at<unsigned char>(row, col) = 0;
		}
	}
	return false; // this triggers backtracking
}

bool CSudokuSolver::FindUnassignedLocation(Mat& sudokuValues,int &row, int &col)
{
	for (row = 0; row < 9; ++ row)
		for (col = 0; col < 9; ++col)
			if (sudokuValues.at<unsigned char>(row, col) == 0)
				return true;
	return false;
}

bool CSudokuSolver::UsedInRow(Mat& sudokuValues, int row, int num)
{
	for (int col = 0; col < 9; ++col)
		if (sudokuValues.at<unsigned char>(row, col) == num)
			return true;
	return false;
}

bool CSudokuSolver::UsedInCol(Mat& sudokuValues, int col, int num)
{
	for (int row = 0; row < 9; ++row)
		if (sudokuValues.at<unsigned char>(row, col) == num)
			return true;
	return false;
}

bool CSudokuSolver::UsedInBox(Mat& sudokuValues, int boxStartRow, int boxStartCol, int num)
{
	for (int row = 0; row < 3; ++ row)
		for (int col = 0; col < 3; ++ col)
			if (sudokuValues.at<unsigned char>(row + boxStartRow,col + boxStartCol) == num)
				return true;
	return false;
}

bool CSudokuSolver::isSafe(Mat& sudokuValues, int row, int col, int num)
{
	return !UsedInRow(sudokuValues,row, num) &&
		!UsedInCol(sudokuValues,col, num) &&
		!UsedInBox(sudokuValues,row - row % 3, col - col % 3, num);
}