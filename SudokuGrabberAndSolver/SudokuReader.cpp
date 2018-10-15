#include "stdafx.h"
#include "SudokuReader.h"
#include "SudokuGrabberAndSolverDlg.h"

SudokuReader::SudokuReader(CSudokuGrabberAndSolverDlg* pParent)
{
	m_pParent = pParent;
}


SudokuReader::~SudokuReader()
{
}

void SudokuReader::ReadSudoku(const char*  strFilename, Mat& sudokuValues)
{

	Mat sudoku = imread(strFilename, 1);
	m_pParent->DrawOnPanel(360, 360, sudoku);
	//////////////////////////////////////////////////////////////////

	Mat outerBox = Mat(sudoku.size(), CV_8UC1);
	//Preprocessing the image
	cvtColor(sudoku, sudoku, CV_BGR2GRAY);
	GaussianBlur(sudoku, sudoku, Size(5, 5), 0);
	adaptiveThreshold(sudoku, outerBox, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
	bitwise_not(outerBox, outerBox);
	Mat kernel = (Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);
	dilate(outerBox, outerBox, kernel);
	//Finding the biggest blob
	int count = 0;
	int max = -1;

	Point maxPt;

	for (int y = 0; y < outerBox.size().height; y++)
	{
		uchar *row = outerBox.ptr(y);
		for (int x = 0; x < outerBox.size().width; x++)
		{
			if (row[x] >= 128)
			{

				int area = floodFill(outerBox, Point(x, y), CV_RGB(0, 0, 64));

				if (area > max)
				{
					maxPt = Point(x, y);
					max = area;
				}
			}
		}
	}

	floodFill(outerBox, maxPt, CV_RGB(255, 255, 255));

	for (int y = 0; y < outerBox.size().height; y++)
	{
		uchar *row = outerBox.ptr(y);
		for (int x = 0; x < outerBox.size().width; x++)
		{
			if (row[x] == 64 && x != maxPt.x && y != maxPt.y)
			{
				int area = floodFill(outerBox, Point(x, y), CV_RGB(0, 0, 0));
			}
		}
	}
	erode(outerBox, outerBox, kernel);

	//Detecting lines
	std::vector<Vec2f> lines;
	HoughLines(outerBox, lines, 1, CV_PI / 180, 200);
	mergeRelatedLines(&lines, sudoku); // Add this line
	for (int i = 0; i < lines.size(); i++)
	{
		drawLine(lines[i], outerBox, CV_RGB(0, 0, 128));
	}
	//Finding extreme lines
	// Now detect the lines on extremes
	Vec2f topEdge = Vec2f(1000, 1000);
	double topYIntercept = 100000, topXIntercept = 0;
	Vec2f bottomEdge = Vec2f(-1000, -1000);
	double bottomYIntercept = 0, bottomXIntercept = 0;
	Vec2f leftEdge = Vec2f(1000, 1000);
	double leftXIntercept = 100000, leftYIntercept = 0;
	Vec2f rightEdge = Vec2f(-1000, -1000);
	double rightXIntercept = 0, rightYIntercept = 0;

	for (int i = 0; i < lines.size(); i++)
	{

		Vec2f current = lines[i];

		float p = current[0];
		float theta = current[1];

		if (p == 0 && theta == -100)
			continue;

		double xIntercept, yIntercept;
		xIntercept = p / cos(theta);
		yIntercept = p / (cos(theta)*sin(theta));

		if (theta > CV_PI * 80 / 180 && theta < CV_PI * 100 / 180)
		{
			if (p < topEdge[0])
				topEdge = current;

			if (p > bottomEdge[0])
				bottomEdge = current;
		}
		else if (theta<CV_PI * 10 / 180 || theta>CV_PI * 170 / 180)
		{
			if (xIntercept > rightXIntercept)
			{
				rightEdge = current;
				rightXIntercept = xIntercept;
			}
			else if (xIntercept <= leftXIntercept)
			{
				leftEdge = current;
				leftXIntercept = xIntercept;
			}
		}
	}
	drawLine(topEdge, sudoku, CV_RGB(0, 0, 0));
	drawLine(bottomEdge, sudoku, CV_RGB(0, 0, 0));
	drawLine(leftEdge, sudoku, CV_RGB(0, 0, 0));
	drawLine(rightEdge, sudoku, CV_RGB(0, 0, 0));

	Point left1, left2, right1, right2, bottom1, bottom2, top1, top2;

	int height = outerBox.size().height;
	int width = outerBox.size().width;

	if (leftEdge[1] != 0)
	{
		left1.x = 0;        left1.y = leftEdge[0] / sin(leftEdge[1]);
		left2.x = width;    left2.y = -left2.x / tan(leftEdge[1]) + left1.y;
	}
	else
	{
		left1.y = 0;        left1.x = leftEdge[0] / cos(leftEdge[1]);
		left2.y = height;    left2.x = left1.x - height * tan(leftEdge[1]);

	}

	if (rightEdge[1] != 0)
	{
		right1.x = 0;        right1.y = rightEdge[0] / sin(rightEdge[1]);
		right2.x = width;    right2.y = -right2.x / tan(rightEdge[1]) + right1.y;
	}
	else
	{
		right1.y = 0;        right1.x = rightEdge[0] / cos(rightEdge[1]);
		right2.y = height;    right2.x = right1.x - height * tan(rightEdge[1]);

	}

	bottom1.x = 0;    bottom1.y = bottomEdge[0] / sin(bottomEdge[1]);
	bottom2.x = width; bottom2.y = -bottom2.x / tan(bottomEdge[1]) + bottom1.y;

	top1.x = 0;        top1.y = topEdge[0] / sin(topEdge[1]);
	top2.x = width;    top2.y = -top2.x / tan(topEdge[1]) + top1.y;

	// Next, we find the intersection of  these four lines
	double leftA = left2.y - left1.y;
	double leftB = left1.x - left2.x;

	double leftC = leftA * left1.x + leftB * left1.y;

	double rightA = right2.y - right1.y;
	double rightB = right1.x - right2.x;

	double rightC = rightA * right1.x + rightB * right1.y;

	double topA = top2.y - top1.y;
	double topB = top1.x - top2.x;

	double topC = topA * top1.x + topB * top1.y;

	double bottomA = bottom2.y - bottom1.y;
	double bottomB = bottom1.x - bottom2.x;

	double bottomC = bottomA * bottom1.x + bottomB * bottom1.y;

	// Intersection of left and top
	double detTopLeft = leftA * topB - leftB * topA;

	CvPoint ptTopLeft = cvPoint((topB*leftC - leftB * topC) / detTopLeft, (leftA*topC - topA * leftC) / detTopLeft);

	// Intersection of top and right
	double detTopRight = rightA * topB - rightB * topA;

	CvPoint ptTopRight = cvPoint((topB*rightC - rightB * topC) / detTopRight, (rightA*topC - topA * rightC) / detTopRight);

	// Intersection of right and bottom
	double detBottomRight = rightA * bottomB - rightB * bottomA;
	CvPoint ptBottomRight = cvPoint((bottomB*rightC - rightB * bottomC) / detBottomRight, (rightA*bottomC - bottomA * rightC) / detBottomRight);// Intersection of bottom and left
	double detBottomLeft = leftA * bottomB - leftB * bottomA;
	CvPoint ptBottomLeft = cvPoint((bottomB*leftC - leftB * bottomC) / detBottomLeft, (leftA*bottomC - bottomA * leftC) / detBottomLeft);

	int maxLength = (ptBottomLeft.x - ptBottomRight.x)*(ptBottomLeft.x - ptBottomRight.x) + (ptBottomLeft.y - ptBottomRight.y)*(ptBottomLeft.y - ptBottomRight.y);
	int temp = (ptTopRight.x - ptBottomRight.x)*(ptTopRight.x - ptBottomRight.x) + (ptTopRight.y - ptBottomRight.y)*(ptTopRight.y - ptBottomRight.y);

	if (temp > maxLength)
		maxLength = temp;

	temp = (ptTopRight.x - ptTopLeft.x)*(ptTopRight.x - ptTopLeft.x) + (ptTopRight.y - ptTopLeft.y)*(ptTopRight.y - ptTopLeft.y);

	if (temp > maxLength)
		maxLength = temp;

	temp = (ptBottomLeft.x - ptTopLeft.x)*(ptBottomLeft.x - ptTopLeft.x) + (ptBottomLeft.y - ptTopLeft.y)*(ptBottomLeft.y - ptTopLeft.y);

	if (temp > maxLength)
		maxLength = temp;

	maxLength = sqrt((double)maxLength);

	Point2f src[4], dst[4];
	src[0] = ptTopLeft;
	dst[0] = Point2f(0, 0);
	src[1] = ptTopRight;
	dst[1] = Point2f(maxLength - 1, 0);
	src[2] = ptBottomRight;
	dst[2] = Point2f(maxLength - 1, maxLength - 1);
	src[3] = ptBottomLeft;
	dst[3] = Point2f(0, maxLength - 1);

	Mat undistorted = Mat(Size(maxLength, maxLength), CV_8UC1);
	warpPerspective(sudoku, undistorted, getPerspectiveTransform(src, dst), Size(maxLength, maxLength));
	///////////////////////
	Mat undistortedThreshed = undistorted.clone();
	adaptiveThreshold(undistorted, undistortedThreshed, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 11, 2);
	//	imshow("temp0", undistortedThreshed);
	//	cvWaitKey(1000);
		//////////////////////////
		// remove guidline
	{
		Mat outerBox1 = undistortedThreshed.clone();
		dilate(outerBox1, outerBox1, kernel);
		int count = 0;
		int max = -1;
		Point maxPt;
		for (int y = 0; y < outerBox1.size().height; y++)
		{
			uchar *row = outerBox1.ptr(y);
			for (int x = 0; x < outerBox1.size().width; x++)
			{
				if (row[x] >= 128)
				{
					int area = floodFill(outerBox1, Point(x, y), CV_RGB(0, 0, 64));
					if (area > max)
					{
						maxPt = Point(x, y);
						max = area;
					}
				}
			}
		}

		floodFill(outerBox1, maxPt, CV_RGB(255, 255, 255));
		for (int y = 0; y < outerBox1.size().height; y++)
		{
			uchar *row = outerBox1.ptr(y);
			for (int x = 0; x < outerBox1.size().width; x++)
			{
				if (row[x] == 64 && x != maxPt.x && y != maxPt.y)
				{
					int area = floodFill(outerBox1, Point(x, y), CV_RGB(0, 0, 0));
				}
			}
		}
		erode(outerBox1, outerBox1, kernel);
		bitwise_not(outerBox1, outerBox1);
		bitwise_and(undistortedThreshed, outerBox1, undistortedThreshed);
	}
	//////////////////////////
	std::vector<ContourWithData> allContoursWithData;
	std::vector<ContourWithData> validContoursWithData;
	Mat matClassificationInts;
	FileStorage fsClassifications("classifications.xml", FileStorage::READ);
	if (fsClassifications.isOpened() == false)
	{
		m_pParent->LogStatus("error, unable to open training classifications file, exiting program\n");
		return;
	}
	fsClassifications["classifications"] >> matClassificationInts;
	fsClassifications.release();
	Mat matTrainingImagesAsFlattenedFloats;
	FileStorage fsTrainingImages("images.xml", FileStorage::READ);
	if (fsTrainingImages.isOpened() == false)
	{
		m_pParent->LogStatus("error, unable to open training images file, exiting program\n");
		return;
	}
	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;
	fsTrainingImages.release();
	Ptr<ml::KNearest>  kNearest(ml::KNearest::create());
	kNearest->train(matTrainingImagesAsFlattenedFloats, ml::ROW_SAMPLE, matClassificationInts);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// make it saperate.
	int dist = ceil((double)maxLength / 9);
	Mat currentCell = Mat(dist, dist, CV_8UC1);
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			sudokuValues.at<unsigned char>(i, j) = 0;

	for (int j = 0; j < 9; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			validContoursWithData.clear();
			allContoursWithData.clear();

			for (int y = 0; y < dist && j*dist + y < undistortedThreshed.cols; y++)
			{

				uchar* ptr = currentCell.ptr(y);

				for (int x = 0; x < dist && i*dist + x < undistortedThreshed.rows; x++)
				{
					ptr[x] = undistortedThreshed.at<uchar>(j*dist + y, i*dist + x);
				}
			}
			Mat newCurrentCell = currentCell.clone();

			std::vector<std::vector<Point> > ptContours;
			std::vector<Vec4i> v4iHierarchy;

			findContours(newCurrentCell, ptContours, v4iHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

			if (ptContours.size() == 1)
			{
				ContourWithData contourWithData;
				contourWithData.ptContour = ptContours[0];
				contourWithData.boundingRect = boundingRect(contourWithData.ptContour);
				contourWithData.fltArea = contourArea(contourWithData.ptContour);
				allContoursWithData.push_back(contourWithData);
			}
			else if (ptContours.size() > 1)
			{// max area 
				float fltMax = 0.0;
				int nMaxi = 0;
				for (int i = 0; i < ptContours.size(); i++)	// can only one ptContour
				{
					ContourWithData contourWithData;
					contourWithData.ptContour = ptContours[i];
					contourWithData.boundingRect = boundingRect(contourWithData.ptContour);
					contourWithData.fltArea = contourArea(contourWithData.ptContour);
					if (contourWithData.fltArea > fltMax)
					{
						fltMax = contourWithData.fltArea;
						nMaxi = i;
					}
				}
				ContourWithData contourWithData;
				contourWithData.ptContour = ptContours[nMaxi];
				contourWithData.boundingRect = boundingRect(contourWithData.ptContour);
				contourWithData.fltArea = contourArea(contourWithData.ptContour);
				allContoursWithData.push_back(contourWithData);
			}
			else
			{
				m_pParent->LogStatus("0");
				continue;
			}

			for (int i = 0; i < allContoursWithData.size(); i++)
			{
				if (allContoursWithData[i].checkIfContourIsValid())
				{
					validContoursWithData.push_back(allContoursWithData[i]);
				}
				else
					m_pParent->LogStatus("0");
			}
			if (validContoursWithData.size() != 0)
			{
				Mat matROI = newCurrentCell(validContoursWithData[0].boundingRect);
				Mat matROIResized;
				resize(matROI, matROIResized, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));
				Mat matROIFloat;
				matROIResized.convertTo(matROIFloat, CV_32FC1);
				Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
				Mat matCurrentChar(0, 0, CV_32F);
				kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);
				float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
				int nCurchar = int(fltCurrentChar) - 48;
				sudokuValues.at<unsigned char>(j, i) = nCurchar;
				m_pParent->LogStatus("%d", nCurchar);
			}
		}
		m_pParent->LogStatus("\n");
	}
}

void SudokuReader::drawLine(Vec2f line, Mat &img, Scalar rgb)
{
	if (line[1] != 0)
	{
		float m = -1 / tan(line[1]);

		float c = line[0] / sin(line[1]);

		cv::line(img, cv::Point(0, c), cv::Point(img.size().width, m*img.size().width + c), rgb);
	}
	else
	{
		cv::line(img, cv::Point(line[0], 0), cv::Point(line[0], img.size().height), rgb);
	}

}
void SudokuReader::mergeRelatedLines(std::vector<cv::Vec2f> *lines, cv::Mat &img)
{
	std::vector<cv::Vec2f>::iterator current;
	for (current = lines->begin(); current != lines->end(); current++)
	{
		if ((*current)[0] == 0 && (*current)[1] == -100)
			continue;
		float p1 = (*current)[0];
		float theta1 = (*current)[1];
		cv::Point pt1current, pt2current;
		if (theta1 > CV_PI * 45 / 180 && theta1 < CV_PI * 135 / 180)
		{
			pt1current.x = 0;

			pt1current.y = p1 / sin(theta1);

			pt2current.x = img.size().width;
			pt2current.y = -pt2current.x / tan(theta1) + p1 / sin(theta1);
		}
		else
		{
			pt1current.y = 0;

			pt1current.x = p1 / cos(theta1);

			pt2current.y = img.size().height;
			pt2current.x = -pt2current.y / tan(theta1) + p1 / cos(theta1);

		}
		std::vector<cv::Vec2f>::iterator    pos;
		for (pos = lines->begin(); pos != lines->end(); pos++)
		{
			if (*current == *pos) continue;
			if (fabs((*pos)[0] - (*current)[0]) < 20 && fabs((*pos)[1] - (*current)[1]) < CV_PI * 10 / 180)
			{
				float p = (*pos)[0];
				float theta = (*pos)[1];
				cv::Point pt1, pt2;
				if ((*pos)[1] > CV_PI * 45 / 180 && (*pos)[1] < CV_PI * 135 / 180)
				{
					pt1.x = 0;
					pt1.y = p / sin(theta);
					pt2.x = img.size().width;
					pt2.y = -pt2.x / tan(theta) + p / sin(theta);
				}
				else
				{
					pt1.y = 0;
					pt1.x = p / cos(theta);
					pt2.y = img.size().height;
					pt2.x = -pt2.y / tan(theta) + p / cos(theta);
				}
				if (((double)(pt1.x - pt1current.x)*(pt1.x - pt1current.x) + (pt1.y - pt1current.y)*(pt1.y - pt1current.y) < 64 * 64) &&
					((double)(pt2.x - pt2current.x)*(pt2.x - pt2current.x) + (pt2.y - pt2current.y)*(pt2.y - pt2current.y) < 64 * 64))
				{
					// Merge the two
					(*current)[0] = ((*current)[0] + (*pos)[0]) / 2;

					(*current)[1] = ((*current)[1] + (*pos)[1]) / 2;

					(*pos)[0] = 0;
					(*pos)[1] = -100;
				}
			}
		}
	}
}