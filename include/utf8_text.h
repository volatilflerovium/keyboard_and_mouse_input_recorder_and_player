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
#ifndef _UTF8_TEXT_H
#define _UTF8_TEXT_H

#include "debug_utils.h"

#include <functional>

#include <cstring>
#include <string>

#include <cwchar>

#define U8(x) u8##x

//====================================================================

class UTF8Char
{
	public:
		UTF8Char()=default;

		UTF8Char(const char8_t* u8str)
		{
			loadChar(u8str);
		}

		UTF8Char(const char* cstr)
		: UTF8Char(reinterpret_cast<const char8_t*>(cstr))
		{
		}
		
		void loadChar(const char8_t* u8str, int bts)
		{
			loadChar(u8str);
			if(bts!=m_size){
				m_size=0;
			} 
		}
		
		void loadChar(const char8_t* str)
		{
			m_size=utf8CodeUnit(*str);

			if(m_size>0){
				m_utf8[1]='\0';
				m_utf8[2]='\0';
				m_utf8[3]='\0';
				m_utf8[4]='\0';
				std::memcpy(m_utf8, str, m_size*sizeof(char8_t));
			}
		}

		UTF8Char& operator=(const UTF8Char& other)
		{
			std::memcpy(m_utf8, other.m_utf8, 4*sizeof(char8_t));
			return *this;
		}

		UTF8Char& operator=(const char8_t* utf8Char)
		{
			loadChar(utf8Char);
			return *this;
		}

		UTF8Char& operator=(const char* utf8Char)
		{
			loadChar(reinterpret_cast<const char8_t*>(utf8Char));
			return *this;
		}

		static unsigned char utf8CodeUnit(const char8_t ch8);

		/*
		 * Take a hex string and return the respective utf8 char
		 *
		 * For example "0001F607" (\U0001F607) return the UTF8Char for 😇
		 * 
		 * */
		static UTF8Char hex2UTF8Char(const char* hexSymbol);

		bool isValid() const
		{
			return m_size>0;
		}

		bool isNullChar() const
		{
			return m_size==1 && m_utf8[0]==0;
		}

		void print()
		{
			dbg(reinterpret_cast<const char*>(m_utf8), " : ", (int)m_size);
		}

		const char8_t* asConst8Char() const
		{
			return m_utf8;
		}

		const char* asConstChar() const
		{
			return reinterpret_cast<const char*>(m_utf8);
		}

		unsigned char size() const
		{
			return m_size;
		}

		bool operator==(const UTF8Char& other)
		{
			if(m_size==other.m_size){
				return std::memcmp(m_utf8, other.m_utf8, m_size*sizeof(char8_t))==0;
			}
			return false;
		}

	private:
		char8_t m_utf8[5]={'\0', '\0', '\0','\0', '\0'};
		unsigned char m_size{0}; // how many char8_t the utf8 symbol is made of
};


inline bool operator<(const UTF8Char& l, const UTF8Char& other)
{
	unsigned char t=l.size()<other.size()? l.size() : other.size();
	return std::memcmp(l.asConst8Char(), other.asConst8Char(), t*sizeof(char8_t))<0;
}

inline std::ostream& operator<<(std::ostream& out, const UTF8Char& utf8Char)
{
	out<<utf8Char.asConstChar();
	return out;
}

//====================================================================

class UTF8_Text
{
	public:
		UTF8_Text(std::u8string&& str)
		:m_text(std::forward<std::u8string>(str))
		, m_strLen(0)
		{
			init(m_text.data());
		}
		
		UTF8_Text(const char8_t* str)
		:m_text(str)
		, m_strLen(0)
		{
			init(str);
		}

		~UTF8_Text()=default;

		typedef std::function<void(const char8_t*, int)> proArraySymbol;

		void iterar(proArraySymbol cbk)
		{
			for(auto& [a,b] : m_chars){
				cbk(m_text.data()+a, b);
			}
		}

		void find(const char8_t* str, std::function<void(int)> cbk=nullptr);

		size_t totalWords() const
		{
			return m_words.size();
		}

		size_t length() const
		{
			return m_chars.size();
		}

		void getWord(size_t idx, proArraySymbol cbk)
		{
			if(idx<m_words.size()){
				int p=m_words[idx].first;
				int l=m_words[idx].second;
				cbk(m_text.data()+p, l);
			}
		}

		std::u8string substr(uint start, uint length) const;

		void debug();

		std::u8string substr(uint start) const
		{
			return substr(start, m_strLen-start);
		}

		const char8_t* c_str() const
		{
			return m_text.c_str();
		}

	private:
		const std::u8string m_text;
		// the position (the positio of each utf8 character in the array char8_t[]
		// the second is the size in bites (up to 4) of each utf8 character
		std::vector<std::pair<int, unsigned char>> m_chars;

		//position of the word in the array m_text, second is the length of the word
		// in char8_t (not as in utf8 characters, for example "a" and "£" both has same )
		// utf8 character length but different char8_t length ("a" is one and "£" is 2)
		std::vector<std::pair<int, int>> m_words;
		size_t m_strLen;

		void init(const char8_t* str);

};

//====================================================================

#endif
