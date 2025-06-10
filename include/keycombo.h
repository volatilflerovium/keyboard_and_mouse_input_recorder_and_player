/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* struct KeyCombo                                                    *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    04-06-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _KEYCOMBO_H
#define _KEYCOMBO_H

#include "key_index.h"

//====================================================================

typedef void(*IFUCN_CBK)(int);

struct KeyCombo
{
	KeyCombo()=default;

	KeyCombo(int a, int b=-1)
	{
		m_data[0]=a;
		m_data[1]=b;
	}

	KeyCombo(int (&indexes)[MAX_HID_CODES])
	{
		for(int i=0; i<MAX_HID_CODES; i++){
			if(indexes[i]<0){
				break;
			}
			m_data[i]=indexes[i];
		}
	}

	KeyCombo(const KeyCombo& other)
	{
		for(int i=0; i<MAX_HID_CODES; i++){
			m_data[i]=other[i];
		}
	}

	KeyCombo& operator=(const KeyCombo& other)
	{
		for(int i=0; i<MAX_HID_CODES; i++){
			m_data[i]=other[i];
		}
		return *this;
	}

	void pushBack(int code);
	void pop();

	void reset();

	int operator[](int idx) const
	{
		if(idx>=MAX_HID_CODES){
			throw "Index out of range.";
		}
		return m_data[idx];
	}

	void apply(IFUCN_CBK cbk) const
	{
		for(unsigned char i=0; i<MAX_HID_CODES; i++){
			if(m_data[i]==-1){
				break;
			}
			cbk(m_data[i]);
		}
	}

	bool isValid() const
	{
		return m_data[0]>-1;
	}

	int* data()
	{
		return m_data;
	}

	int m_data[MAX_HID_CODES]={-1, -1, -1, -1, -1, -1};
};

//--------------------------------------------------------------------

inline void KeyCombo::pushBack(int code)
{
	for(unsigned char i=0; i<MAX_HID_CODES; i++){
		if(m_data[i]==-1 || m_data[i]==code){
			m_data[i]=code;
			break;
		}
	}
}

//--------------------------------------------------------------------

inline void KeyCombo::pop()
{
	for(unsigned char i=MAX_HID_CODES-1; i>=0 && i<255; i--){
		if(m_data[i]>-1){
			m_data[i]=-1;
			break;
		}
	}
}

//--------------------------------------------------------------------

inline void KeyCombo::reset()
{
	for(unsigned char i=0; i<MAX_HID_CODES; i++){
		m_data[i]=-1;
	}
}

//====================================================================

#endif
