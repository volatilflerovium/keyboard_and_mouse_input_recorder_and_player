/*********************************************************************
* SimpleImageDifference class                                        *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef IMAGE_DIFF_RANK_H
#define IMAGE_DIFF_RANK_H

#include <cstring>

//====================================================================

class SimpleImageDifference 
{
	public:
		SimpleImageDifference();

		virtual ~SimpleImageDifference();
		

		virtual void loadBaseImage(const char* baseImageName)
		{
			loadBaseImage(baseImageName, true, true);
		}

		// Maximum value for @param threshold is 441.
		virtual bool isSimilar(const char* sampleImageName, unsigned int threshold, unsigned int sensitivity=100)
		{
			return getDifference(sampleImageName, threshold)<sensitivity;
		}

		virtual size_t getDifference(const char* sampleImageName, unsigned int range);

		virtual void loadBaseImage(const char* baseImageName, bool sharping, bool denoise);

	private:
		class CVMAT;
		CVMAT* m_impl;
};

//====================================================================

#endif
