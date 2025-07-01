/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* class KeyboardEmulatorI                                            *
* class DummyKeyboard                                                *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include "utf8_text.h"

#include <cmath>

//====================================================================

unsigned char UTF8Char::utf8CodeUnit(const char8_t ch8)
{
	if(ch8 < 0b1000'0000) { // 7-bit code unit
		return 1;
	}

	if(ch8< 0b1100'0000) { // continuation byte in this context is invalid
		return 0;
	}

	if(ch8 < 0b1110'0000) {
		return 2;
	}

	if(ch8 < 0b1111'0000) {
		return 3;
	}

	if(ch8 < 0b1111'1000) {
		return 4;
	}

	// else out of range for code unit
	return 0;
}

//--------------------------------------------------------------------

UTF8Char UTF8Char::hex2UTF8Char(const char* hexSymbol)
{
	static auto convert=[](char b){
		if(b=='0'){return 0;}
		if(b=='1'){return 1;}
		if(b=='2'){return 2;}
		if(b=='3'){return 3;}
		if(b=='4'){return 4;}
		if(b=='5'){return 5;}
		if(b=='6'){return 6;}
		if(b=='7'){return 7;}
		if(b=='8'){return 8;}
		if(b=='9'){return 9;}
		if(b=='A' || b=='a'){return 10;}
		if(b=='B' || b=='b'){return 11;}
		if(b=='C' || b=='c'){return 12;}
		if(b=='D' || b=='d'){return 13;}
		if(b=='E' || b=='e'){return 14;}
		//if(b=='F' || b=='f'){
		return 15;
	};

	uint32_t val=0;
	size_t l=std::strlen(hexSymbol);
	for(size_t i=0; i<l; i++){
		val+=std::pow(16, l-(i+1)) * convert(hexSymbol[i]);
	}

	char8_t data[5]={'\0', '\0','\0','\0','\0'};
	
	if(val<(1<<7)){
		data[0]=(63 & val);
	}
	else if(val<(1<<11)){
		data[1]=(63 & val) | 1<<7;
		data[0]=(63 & (val>>6) ) | 6<<5;
	}
	else if(val<(1<<16)){
		data[2]=(63 & val) | 1<<7;
		data[1]=(63 & (val>>6)) | 1<<7;
		data[0]=(63 & (val>>12) ) | 14<<4;
	}
	else if(val<(1<<21)){
		data[3]=(63 & val) | 1<<7;
		data[2]=(63 & (val>>6)) | 1<<7;
		data[1]=(63 & (val>>12) ) | 1<<7;
		data[0]=(63 & (val>>18) ) | 30<<3;
	}
	else{
		dbg("bad hex");
	}

	return data;
}

//====================================================================

void UTF8_Text::find(const char8_t* str, std::function<void(int)> cbk)
{
	unsigned char l=UTF8Char(str).size();
	for(auto& [a,b] : m_chars){
		if(b==l){
			if(std::memcmp(str, &m_text.c_str()[a], b)==0){
				if(cbk){
					cbk(a);
				}
				//dbg("found: ", reinterpret_cast<const char*>(str), " at: ", a, " : ", b);
			}
		}
	}
}

//--------------------------------------------------------------------

std::u8string UTF8_Text::substr(uint start, uint length) const
{
	if(m_chars.size()<start){
		return u8"";
	}
	uint s=0;
	for(size_t i=start; i<start+length; i++){
		s+=m_chars[i].second;
	}
	std::u8string result(s, '\0');
	std::memcpy(result.data(), m_text.data()+m_chars[start].first, s*sizeof(char8_t));
	return result;
}

//--------------------------------------------------------------------

void UTF8_Text::debug()
{
	dbg("length: ", m_strLen);
	for(auto& [a,b] : m_chars){
		dbg("symbol position: ", a, " bytes ", b);
	}

	for(auto& [p, l] : m_words){
		dbg("word at: ", p, " length: ", l);
	}
}

//--------------------------------------------------------------------

void UTF8_Text::init(const char8_t* str)
{
	int wp=0;
	int wl=0;
	unsigned char b=0;
	size_t left=0;
	int p=0;
	while(*(str+p)){
		b=UTF8Char::utf8CodeUnit(*(str+p));
		if(b==0){
			m_strLen= -1;
			break;
		}

		if(std::memcmp(" ", str+p, 1)!=0){
			if(0==wl){
				wp=p;
			}
			wl+=b;
		}
		else{
			if(wl>0){
				m_words.emplace_back(wp, wl);
			}
			wl=0;
			
		}
		m_chars.emplace_back(left, b);

		++m_strLen;
		p+=b;

		left+=b;
	}

	if(m_strLen>1 && wl>0){
		m_words.emplace_back(wp, wl);
	}
}

//====================================================================

