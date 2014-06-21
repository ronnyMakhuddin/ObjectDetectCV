#include "StdAfx.h"
#include "HarrisDetector.h"


CHarrisDetector::CHarrisDetector(void)
	:m_nNeighbourhood(3)
	,m_nAperture(3)
	,m_dk(0.0)
	,m_dThreshold(0.01)
	,m_nNonMaxSize(3)
{
}


CHarrisDetector::~CHarrisDetector(void)
{
}


//Compute Harris Corners
void CHarrisDetector::DetectHarrisCorner(const cv::Mat& mtInputFrame)
{
	//Harris Computation
	cv::cornerHarris(mtInputFrame, m_matCornerStrength, m_nNeighbourhood, m_nAperture, m_dk);
	//Internal threshold compuation
	double dminStrength;
	cv::minMaxLoc(m_matCornerStrength, &dminStrength, & m_dMaxStrength);

	//Local Maxima Detection
	cv::Mat mtDilated;
	cv::dilate(m_matCornerStrength, mtDilated, cv::Mat());
	cv::compare(m_matCornerStrength,mtDilated, m_matLocalMax, cv::CMP_EQ);


}

//Get the corner map from the compuated harris values
cv::Mat CHarrisDetector::GetCornerMap(double dQualityLevel)
{
	cv::Mat mtCornerMap;
	//Thresholding the corner strength
	m_dThreshold = dQualityLevel* m_dMaxStrength;
	cv::threshold(m_matCornerStrength, m_matCornerThreshold,m_dThreshold, 255, cv::THRESH_BINARY);

	//convert to 8bit image
	m_matCornerThreshold.convertTo(mtCornerMap, CV_8U);
	//Non Maxima suppression
	cv::bitwise_and(mtCornerMap, m_matLocalMax, mtCornerMap);
	return mtCornerMap;
}

//Get the feature points from computed harris values
void CHarrisDetector::GetCorners(std::vector<cv::Point> &points, double dQualityLevel)
{
	//Get the cornermap
	cv::Mat mtCornerMap = GetCornerMap(dQualityLevel);

	//Get the corners
	GetCorners(points, mtCornerMap);

}

//Get the feature points from compuated cornermap
void CHarrisDetector::GetCorners(std::vector<cv::Point> &points, const cv::Mat& mtCornerMap)
{
	//Iterate over the pixels to obtain all features
	for(int y =0; y < mtCornerMap.rows; y++)
	{
		const uchar* rowPtr = mtCornerMap.ptr<uchar>(y);
		for(int x=0; x < mtCornerMap.cols; x++)
		{
			//if it is a feature point
			if(rowPtr[x]){
				points.push_back(cv::Point(x,y));
			}
		}
	}

}

//Draw Circles at feature point locations on an image
void CHarrisDetector::DrawOnImage(cv::Mat &image,const std::vector<cv::Point> &points, cv::Scalar  color, int nRadius, int nThinkness)
{
	std::vector<cv::Point>::const_iterator it = points.begin();
	//for all corners
	while(it != points.end())
	{
		//draw a circle at each corner location
		cv::circle(image, *it, nRadius, color, nThinkness);
		++it;
	}

}


void CHarrisDetector::GetGoodFeaturePoints( cv::Mat& mtInputFrame)
{
	std::vector<cv::KeyPoint> vKeyPoints;
	cv::GoodFeaturesToTrackDetector gftt(1000,0.01,1);
	gftt.detect(mtInputFrame, vKeyPoints);

	DumpKeyPointsToFile(0, vKeyPoints);
	DrawOnImageOpenCV(mtInputFrame, vKeyPoints, cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

}

void CHarrisDetector::GetFASTFeaturePoints( cv::Mat& mtInputFrame)
{
	std::vector<cv::KeyPoint> vKeyPoints;
	cv::FastFeatureDetector fast(40);
	fast.detect(mtInputFrame, vKeyPoints);

	DumpKeyPointsToFile(1, vKeyPoints);
	DrawOnImageOpenCV(mtInputFrame,vKeyPoints, cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

}
void CHarrisDetector::GetSURFFeaturePoints( cv::Mat& mtInputFrame)
{
	std::vector<cv::KeyPoint> vKeypoints;
	cv::SurfFeatureDetector surf(2500.);
	surf.detect(mtInputFrame, vKeypoints);

	DumpKeyPointsToFile(2, vKeypoints);
	DrawOnImageOpenCV(mtInputFrame,vKeypoints, cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
}

void CHarrisDetector::GetSIFTFeaturePoints( cv::Mat& mtInputFrame)
{
	std::vector<cv::KeyPoint> vKeypoints;
	cv::SiftFeatureDetector sift(0.03, 10.);
	sift.detect(mtInputFrame, vKeypoints);
	DumpKeyPointsToFile(3, vKeypoints);
	DrawOnImageOpenCV(mtInputFrame, vKeypoints, cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

}

void CHarrisDetector::DumpKeyPointsToFile(int nFeatureExtractMode, std::vector<cv::KeyPoint> vKeypoints)
{
	std::string szFileName, szSectionName;

	switch(nFeatureExtractMode)
	{
	case 0 : szFileName = "GOODKEY.yml"; szSectionName="GoodFeatureExtractor" ; break;
	case 1 : szFileName = "FASTKEY.yml"; szSectionName="FastFeatureExtractor"; break;
	case 2 : szFileName = "SURFKEY.yml"; szSectionName="SurfFeatureExtractor"; break;
	case 3: szFileName  = "SIFTKEY.yml"; szSectionName="SiftFeatureExtratctor"; break;
	default:
		 szFileName = "Good.yml"; szSectionName="GoodFeatureExtractor"; break;

	}
	cv::FileStorage fileStorage(szFileName, cv::FileStorage::WRITE);
	cv::write(fileStorage, szSectionName,vKeypoints);
	fileStorage.release();
}

void CHarrisDetector::DumpDescriptorsToFile(int nFeatureExtractMode, cv::Mat& mtDesc)
{
	std::string szFileName, szSectionName;

	switch(nFeatureExtractMode)
	{
	case 0 : szFileName = "GOODDESC.yml"; szSectionName="GoodFeatureExtractor" ; break;
	case 1 : szFileName = "FASTDESC.yml"; szSectionName="FastFeatureExtractor"; break;
	case 2 : szFileName = "SURFDESC.yml"; szSectionName="SurfFeatureExtractor"; break;
	case 3: szFileName  = "SIFTDESC.yml"; szSectionName="SiftFeatureExtratctor"; break;
	default:
		 szFileName = "GoodDesc.yml"; szSectionName="GoodFeatureExtractor"; break;

	}
	cv::FileStorage fileStorage(szFileName, cv::FileStorage::WRITE);
	cv::write(fileStorage, szSectionName,mtDesc);
	fileStorage.release();
}
void CHarrisDetector::DrawOnImageOpenCV( cv::Mat& mtInputFrame, std::vector<cv::KeyPoint> keypoints, int flag)
{
	cv::drawKeypoints(mtInputFrame, keypoints, mtInputFrame, cv::Scalar(255,255,255),flag);

}

cv::Mat  CHarrisDetector::CompareTwoImages(cv::Mat& mtImageSrc, cv::Mat& mtImageDb)
{
	cv::SurfDescriptorExtractor surfDesc;
	cv::Mat desc1, desc2;
	std::vector<cv::KeyPoint> vKeyPoints1, vKeyPoints2;
	/*
	surfDesc.compute(mtImageSrc, vKeyPoints1, desc1);
	surfDesc.compute(mtImageDb, vKeyPoints2, desc2);

	cv::BFMatcher matcher(cv::NORM_L2, false);
	std::vector<cv::DMatch> matches;
	matcher.match(desc1, desc2, matches);

	if(matches.size() > 25)
	{
		std::nth_element(matches.begin(), matches.begin()+24, matches.end());
		matches.erase(matches.begin()+25, matches.end());
	}

	cv::Mat imageMatches;
	cv::drawMatches(mtImageSrc,vKeyPoints1, mtImageDb, vKeyPoints2,matches, imageMatches, cv::Scalar(255,255,255));
	*/

	 //-- Step 1: Detect the keypoints and generate their descriptors using SURF
     int minHessian = 400;
     cv::SURF surf( minHessian );

     std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
     cv::Mat descriptors_1, descriptors_2;

     surf( mtImageSrc, cv::Mat(), keypoints_1, descriptors_1, false );
     surf( mtImageDb, cv::Mat(), keypoints_2, descriptors_2, false );


     //-- Step 3: Matching descriptor vectors with a brute force matcher
     cv::BFMatcher matcher( cv::NORM_L2, false );
     std::vector< cv::DMatch > matches;
     matcher.match( descriptors_1, descriptors_2, matches );

     //-- Draw matches
     cv::Mat img_matches;
     cv::drawMatches( mtImageSrc, keypoints_1, mtImageDb, keypoints_2, matches, img_matches );

	 DumpKeyPointsToFile(2, keypoints_1);
	 DumpDescriptorsToFile(2,descriptors_1);
	return img_matches;
}