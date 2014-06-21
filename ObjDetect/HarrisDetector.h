#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>


class CHarrisDetector
{
public:
	CHarrisDetector(void);
	~CHarrisDetector(void);

private:
	cv::Mat m_matCornerStrength;
	cv::Mat m_matCornerThreshold;
	cv::Mat m_matLocalMax;
	int m_nNeighbourhood;
	int m_nAperture;
	double m_dk; //harris parameter
	double m_dMaxStrength;
	double m_dThreshold;
	int  m_nNonMaxSize;
	cv::Mat m_kernel;

public:
	void DetectHarrisCorner(const cv::Mat& mtInputFrame);
	cv::Mat GetCornerMap(double dQualityLevel);
	void GetCorners(std::vector<cv::Point> &points, double dQualityLevel);
	void GetCorners(std::vector<cv::Point> &points, const cv::Mat& cornerMap);
	void DrawOnImage(cv::Mat &image,const std::vector<cv::Point> &points, cv::Scalar  color = cv::Scalar(255,255,255), int nRadius = 3, int nThinkness =2);


	//Good Feature Detector
	void GetGoodFeaturePoints( cv::Mat& mtInputFrame);
	void GetFASTFeaturePoints( cv::Mat& mtInputFrame);
	void GetSURFFeaturePoints( cv::Mat& mtInputFrame);
	void GetSIFTFeaturePoints( cv::Mat& mtInputFrame);

	cv::Mat CompareTwoImages(cv::Mat& mtImageSrc, cv::Mat& mtImageDb);
	void DumpKeyPointsToFile(int nFeatureExtractMod, std::vector<cv::KeyPoint> vKeypoints);
	void DumpDescriptorsToFile(int nFeatureExtractMod, cv::Mat& mtDesc);
	void DrawOnImageOpenCV( cv::Mat& mtInputFrame, std::vector<cv::KeyPoint> keypoints, int flag);

};

