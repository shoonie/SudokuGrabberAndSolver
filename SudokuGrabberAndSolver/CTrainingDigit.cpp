#include "stdafx.h"
#include "CTrainingDigit.h"

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include "SudokuGrabberAndSolverDlg.h"

using namespace cv;
using namespace std;
CTrainingDigit::CTrainingDigit(CSudokuGrabberAndSolverDlg* pParent)
{
	m_pParent = pParent;
}

CTrainingDigit::~CTrainingDigit()
{
}

void CTrainingDigit::TrainingDigit(const char*  strFilename)
{
	/////////////////////////////////////////////////
	//check previous file//
	cv::Mat matClassificationInts;
	cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);
	if (fsClassifications.isOpened() == false && m_pParent != nullptr)
	{
		m_pParent->LogStatus("error, unable to open training classifications file, exiting program\n");
		m_pParent->LogStatus("Make file from the scratch\n");
	}
	else
	{
		fsClassifications["classifications"] >> matClassificationInts;
		fsClassifications.release();
	}
	cv::Mat matTrainingImagesAsFlattenedFloats;
	cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);
	if (fsTrainingImages.isOpened() == false && m_pParent != nullptr) {
		m_pParent->LogStatus("error, unable to open training images file, exiting program\n");
		m_pParent->LogStatus("Make file from the scratch\n");
	}
	else
	{
		fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;
		fsTrainingImages.release();
	}
	//////////////////////////////////////////////////////

	Mat imgTrainingNumbers;
	Mat imgGrayscale;
	Mat imgBlurred;
	Mat imgThresh;
	Mat imgThreshCopy;
	vector<std::vector<cv::Point> > ptContours;
	vector<cv::Vec4i> v4iHierarchy;

	vector<int> intValidChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', };

	imgTrainingNumbers = cv::imread(strFilename);

	if (imgTrainingNumbers.empty()) {
		m_pParent->LogStatus("error: image not read from file\n");
		return;
	}
	cv::cvtColor(imgTrainingNumbers, imgGrayscale, CV_BGR2GRAY);	// make grayscale
	cv::GaussianBlur(imgGrayscale, imgBlurred, cv::Size(3, 3), 0);	//gaussian blue
	cv::adaptiveThreshold(imgBlurred, imgThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 7, 2);	// binary.

	imgThreshCopy = imgThresh.clone();
	cv::findContours(imgThreshCopy, ptContours, v4iHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < ptContours.size(); i++) {
		if (cv::contourArea(ptContours[i]) > MIN_CONTOUR_AREA) {
			cv::Rect boundingRect = cv::boundingRect(ptContours[i]);

			cv::rectangle(imgTrainingNumbers, boundingRect, cv::Scalar(0, 0, 255), 2);
			cv::Mat matROI = imgThresh(boundingRect);
			cv::Mat matROIResized;
			cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));

			cv::imshow("matROIResized", matROIResized);

			m_pParent->DrawOnPanel(imgTrainingNumbers.size().width, imgTrainingNumbers.size().height, imgTrainingNumbers);

			int intChar = cv::waitKey(0);

			if (intChar == 27) {
				return;
			}
			else if (std::find(intValidChars.begin(), intValidChars.end(), intChar) != intValidChars.end()) {
				matClassificationInts.push_back(intChar);
				cv::Mat matImageFloat;
				matROIResized.convertTo(matImageFloat, CV_32FC1);
				cv::Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1);
				matTrainingImagesAsFlattenedFloats.push_back(matImageFlattenedFloat);
				m_pParent->LogStatus("%d is learned\n", intChar);
			}
		}
	}

	m_pParent->LogStatus("training complete\n");
	// save learning data to xml file
	cv::FileStorage fsClassifications1("classifications.xml", cv::FileStorage::WRITE);
	if (fsClassifications1.isOpened() == false) {
		m_pParent->LogStatus("error, unable to open training classifications file, exiting program\n");
		return;
	}
	fsClassifications1 << "classifications" << matClassificationInts;
	fsClassifications1.release();
	cv::FileStorage fsTrainingImages1("images.xml", cv::FileStorage::WRITE);
	if (fsTrainingImages1.isOpened() == false) {
		m_pParent->LogStatus("error, unable to open training images file, exiting program\n");
		return;
	}
	fsTrainingImages1 << "images" << matTrainingImagesAsFlattenedFloats;
	fsTrainingImages1.release();
	m_pParent->LogStatus("Learning data is made\n");

	imgTrainingNumbers.release();
	imgGrayscale.release();
	imgBlurred.release();
	imgThresh.release();
	imgThreshCopy.release();

	cv::destroyAllWindows();
}
void CTrainingDigit::TestTraining(const char*  strFilename)
{
	cv::Mat matTestingNumbers = cv::imread(strFilename, 1);

	if (matTestingNumbers.empty())
	{
		m_pParent->LogStatus("error: image not read from file\n");
		return;
	}

	cv::Mat matGrayscale;
	cv::Mat matBlurred;
	cv::Mat matThresh;
	cv::Mat matThreshCopy;
	/////////////////////////////////////////////
	m_pParent->DrawOnPanel(matTestingNumbers.size().width, matTestingNumbers.size().height, matTestingNumbers);
	////////////////////////////////////////////////////////////////
	cv::cvtColor(matTestingNumbers, matGrayscale, CV_BGR2GRAY);
	cv::GaussianBlur(matGrayscale, matBlurred, cv::Size(5, 5), 0);
	cv::adaptiveThreshold(matBlurred, matThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);
	matThreshCopy = matThresh.clone();

	std::vector<std::vector<cv::Point> > ptContours;
	std::vector<cv::Vec4i> v4iHierarchy;


	////////////////////////////////////////
	std::vector<ContourWithData> allContoursWithData;
	std::vector<ContourWithData> validContoursWithData;

	cv::Mat matClassificationInts;
	cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);
	if (fsClassifications.isOpened() == false)
	{
		m_pParent->LogStatus("error, unable to open training classifications file, exiting program\n");
		return;
	}
	fsClassifications["classifications"] >> matClassificationInts;
	fsClassifications.release();
	cv::Mat matTrainingImagesAsFlattenedFloats;

	cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);

	if (fsTrainingImages.isOpened() == false)
	{
		m_pParent->LogStatus("error, unable to open training images file, exiting program\n");
		return;
	}
	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;
	fsTrainingImages.release();
	cv::Ptr<cv::ml::KNearest>  kNearest(cv::ml::KNearest::create());
	kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts);

	//test ///////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////


	cv::findContours(matThreshCopy, ptContours, v4iHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < ptContours.size(); i++)
	{
		ContourWithData contourWithData;
		contourWithData.ptContour = ptContours[i];
		contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);
		contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);
		allContoursWithData.push_back(contourWithData);
	}

	for (int i = 0; i < allContoursWithData.size(); i++)
	{
		if (allContoursWithData[i].checkIfContourIsValid())
		{
			validContoursWithData.push_back(allContoursWithData[i]);
		}
	}
	std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);

	std::string strFinalString;

	for (int i = 0; i < validContoursWithData.size(); i++)
	{
		cv::rectangle(matTestingNumbers, validContoursWithData[i].boundingRect, cv::Scalar(255, 0, 0), 2);
		cv::Mat matROI = matThresh(validContoursWithData[i].boundingRect);
		cv::Mat matROIResized;
		cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));
		cv::Mat matROIFloat;
		matROIResized.convertTo(matROIFloat, CV_32FC1);
		cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
		cv::Mat matCurrentChar(0, 0, CV_32F);
		kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);
		float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
		strFinalString = strFinalString + char(int(fltCurrentChar));
	}

	m_pParent->LogStatus("numbers read = %s\n", strFinalString.c_str());
	matGrayscale.release();
	matBlurred.release();
	matThresh.release();
	matThreshCopy.release();
	cv::waitKey(10);
	cv::destroyAllWindows();
}