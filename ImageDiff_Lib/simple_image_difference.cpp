/*********************************************************************
* SimpleImageDifference class                                        *
* cv::Mat sharpImage(const char*)                                    *
* cv::Mat sharpImage(const cv::Mat&)                                 *
* class SimpleImageDifference::CVMAT                                 *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#include "ImageDiff_Lib/simple_image_difference.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <iomanip>

// sharpen image using "unsharp mask" algorithm
static cv::Mat sharpImage(const char* imageName)
{
	cv::Mat img=cv::imread(imageName);
	cv::Mat blurred; 
	double sigma = 1, threshold = 5, amount = 1;
	cv::GaussianBlur(img, blurred, cv::Size(), sigma, sigma);
	cv::Mat lowContrastMask = abs(img - blurred) < threshold;
	cv::Mat sharpened = img*(1+amount) + blurred*(-amount);
	img.copyTo(sharpened, lowContrastMask);	
	return sharpened;
}

static cv::Mat sharpImage(const cv::Mat& img)
{
	cv::Mat blurred; 
	double sigma = 1, threshold = 5, amount = 1;
	cv::GaussianBlur(img, blurred, cv::Size(), sigma, sigma);
	cv::Mat lowContrastMask = abs(img - blurred) < threshold;
	cv::Mat sharpened = img*(1+amount) + blurred*(-amount);
	img.copyTo(sharpened, lowContrastMask);	
	return sharpened;
}

//--------------------------------------------------------------------

static cv::Mat mkMatAdapter(const char* imageName)
{
	return cv::imread(imageName);
}

static cv::Mat mkMatAdapter(const cv::Mat& imgMat)
{
	return imgMat;
}

//--------------------------------------------------------------------

void PrepareImage(const char* imagePath, cv::Mat& result, bool sharp, bool denoise)
{
	cv::Mat (*cbk)(const char*);
	cbk=sharpImage;
	if(sharp){
		cbk=mkMatAdapter;
	}

	if(denoise){
		cv::GaussianBlur((*cbk)(imagePath), result, cv::Size(5, 5), 0);
	}
	else{
		result=(*cbk)(imagePath);
	}
}

//--------------------------------------------------------------------

void PrepareImage(const cv::Mat& imageMat, cv::Mat& result, bool sharp, bool denoise)
{
	cv::Mat (*cbk)(const cv::Mat&);
	cbk=sharpImage;
	if(sharp){
		cbk=mkMatAdapter;
	}

	if(denoise){
		cv::GaussianBlur((*cbk)(imageMat), result, cv::Size(5, 5), 0);
	}
	else{
		result=(*cbk)(imageMat);
	}
}

//======================================================================

class SimpleImageDifference::CVMAT
{
	public:
		CVMAT()
		: m_sharp(false)
		, m_denoise(false)
		{}

		virtual ~CVMAT()=default;

		void loadBaseImage(const char* baseImageName, bool sharping, bool denoise);
		void loadBaseImage(const cv::Mat& baseImage, bool sharping, bool denoise);

		size_t getDifference(const char* sampleImageName, unsigned int range);
		size_t getDifference(const cv::Mat& sampleImage, unsigned int range);

	private:
		cv::Mat m_img;
		cv::Mat m_foregroundMask;
		bool m_sharp;
		bool m_denoise;
		
		size_t processImage(const cv::Mat& sampleImage, unsigned int range);
		
		struct
		{
			size_t m_total{0};
			size_t m_first{0};
			size_t m_last{0};
			float m_range{0};

			void reset()
			{
				m_total=0;
				m_first=0;
				m_last=0;
				m_range=0;				
			}

			float rank3()
			{
				//std::cout<<std::fixed<<std::setprecision(6)<<m_range<<" total: "<<m_total<<" errors: "<<m_first<<" ";
				//std::cout<<" combi: "<<(100.0*m_first)/m_total<<" # ";
				return m_first;
			}
		} m_data;
};

//--------------------------------------------------------------------

size_t SimpleImageDifference::CVMAT::processImage(const cv::Mat& sampleImg, unsigned int range)
{
	cv::Mat diffImage;
	cv::absdiff(m_img, sampleImg, diffImage);

	m_data.reset();

	m_data.m_total=m_img.rows*m_img.cols;

	if(cv::sum(diffImage)==cv::Scalar(0,0,0)){
		return 0;
	}

	float thres=range*range;
	
	if(thres>255*255*3){
		return m_data.rank3();
	}

	size_t diff=0;
	const cv::Vec3b zeroVect(0,0,0);

	for(int j=0; j<diffImage.rows; ++j){
		for(int i=0; i<diffImage.cols; ++i){
			cv::Vec3b pix = diffImage.at<cv::Vec3b>(j,i);

			if(pix==zeroVect){
				continue;
			}
			float r = pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2];
						
			if(r>thres){////	== sqrt(r)>thres)
				diff++;
			}
		}
	}

	m_data.m_last=diff;
	m_data.m_first=diff;
	m_data.m_range=range;

	return diff;//m_data.rank3();	
}

//--------------------------------------------------------------------

void SimpleImageDifference::CVMAT::loadBaseImage(const cv::Mat& baseImageMat, bool sharping, bool denoise)
{
	m_sharp=sharping;
	m_denoise=denoise;
	PrepareImage(baseImageMat, m_img, m_sharp, m_denoise);
}

//--------------------------------------------------------------------

void SimpleImageDifference::CVMAT::loadBaseImage(const char* baseImagePath, bool sharping, bool denoise)
{
	m_sharp=sharping;
	m_denoise=denoise;
	PrepareImage(baseImagePath, m_img, m_sharp, m_denoise);
}

//--------------------------------------------------------------------

size_t SimpleImageDifference::CVMAT::getDifference(const cv::Mat& sampleImageMat, unsigned int range)
{
	cv::Mat sampleImg;
	PrepareImage(sampleImageMat, sampleImg, m_sharp, m_denoise);

	return processImage(sampleImg, range);
}

//--------------------------------------------------------------------

size_t SimpleImageDifference::CVMAT::getDifference(const char* sampleImagePath, unsigned int range)
{
	cv::Mat sampleImg;
	PrepareImage(sampleImagePath, sampleImg, m_sharp, m_denoise);
	
	return processImage(sampleImg, range);
}

//====================================================================

SimpleImageDifference::SimpleImageDifference()
:m_impl(new CVMAT)
{}

SimpleImageDifference::~SimpleImageDifference()
{
	delete m_impl;
}

void SimpleImageDifference::loadBaseImage(const char* baseImageName, bool sharping, bool denoise)
{
	m_impl->loadBaseImage(baseImageName, sharping, denoise);
}

size_t SimpleImageDifference::getDifference(const char* sampleImageName, unsigned int range)
{
	return m_impl->getDifference(sampleImageName, range);
}

//====================================================================
