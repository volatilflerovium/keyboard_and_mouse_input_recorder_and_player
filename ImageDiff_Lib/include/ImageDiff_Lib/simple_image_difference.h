/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
*
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
