/*********************************************************************
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE. 
* 
* template<int N> class CstrSplit                                    *
* class SimpleSerialization                                          *
* template<int N> class SimpleUnserialization         	            *                                            *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _CSTR_SPLIT_H
#define _CSTR_SPLIT_H

#include <iostream>
#include <cstring>
#include <optional>

#define SEPARATOR "#+{35sdfh4}|{7gkjf29}+#"

//====================================================================

template<int N>
class CstrSplit
{
	public:
		CstrSplit(const char* data, const char* separator)
		: m_length(0)
		{
			m_buffer=new char[std::strlen(data)+1];
			std::memcpy(m_buffer, data, std::strlen(data));
			m_buffer[std::strlen(data)]=0;
			splitData(separator);
		}

		~CstrSplit()
		{
			delete[] m_buffer;
		}

		char* operator[](int i)
		{
			if(i<m_length){
				return m_buffer+m_sck[i];
			}
			else{
				throw "ERROR: index out of range.";
			}
		}

		int chunkSize(int p) const
		{
			return m_chunkSize[p];
		}

		size_t dataSize() const
		{
			return m_length;
		}

		//for debuging purposes	
		void print()
		{
			for(int i=0; i<m_length; i++){
				std::cout<<m_chunkSize[i]<< " :"<< operator[](i)<<"\n";
			}
		}

	private:
		char* m_buffer;
		int m_chunkSize[N];
		int m_sck[N];
		int m_length;

		void splitData(const char* separator);
};

//--------------------------------------------------------------------

template<int N>
void CstrSplit<N>::splitData(const char* separator)
{
	int ck[N];
	const int spl=std::strlen(separator);

	auto helper=[this, pos=0, &ck, spl](int j)mutable{
		m_sck[j]=pos;
		m_chunkSize[j]=ck[j]-pos;
		pos=ck[j]+spl;
	};

	int i=0;
	while(m_buffer[i]!='\0'){
		if(memcmp(m_buffer+i, separator, spl)==0){
			if(m_length<N){
				ck[m_length++]=i;
				m_buffer[i]=0;
				helper(m_length-1);
			}
			i+=spl;
			continue;
		}
		i++;
	}

	if(m_length<N){
		ck[m_length++]=i;
		helper(m_length-1);
	}
}

//====================================================================

template<typename T>
struct ToString
{
	static std::string toStr(T t)
	{
		try {
			return std::to_string(t);
		}
		catch (...) {
			return std::string("!!");
		}
	}
};

template<>
struct ToString<size_t>
{
	static std::string toStr(size_t t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<float>
{
	static std::string toStr(float t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<int>
{
	static std::string toStr(int t)
	{
		return std::to_string(t);
	}
};

template<>
struct ToString<std::string>
{
	static std::string toStr(const std::string& str)
	{
		return str;
	}
};

template<>
struct ToString<const char*>
{
	static std::string toStr(const char* cstr)
	{
		return cstr;
	}
};

template<>
struct ToString<bool>
{
	static std::string toStr(bool a)
	{
		if(a){
			return std::string("true");
		}
		return std::string("false");
	}
};

//====================================================================

template<typename T>
struct FromString
{
};

template<>
struct FromString<int>
{
	static int getFrom(const char* str, int)
	{
		return std::atoi(str);
	}
};

template<>
struct FromString<float>
{
	static int getFrom(const char* str, int)
	{
		return std::atof(str);
	}
};

template<>
struct FromString<const char*>
{
	static const char* getFrom(const char* str, int)
	{
		return str;
	}
};

template<>
struct FromString<std::string>
{
	static std::string getFrom(const char* str, int length)
	{
		return std::string(str, length);
	}
};

template<>
struct FromString<bool>
{
	static bool getFrom(const char* str, int)
	{
		if(std::memcmp(str, "true", 4)==0){
			return true;
		}
		return false;
	}
};

template<>
struct FromString<std::u8string>
{
	static std::u8string getFrom(const char* str, int)
	{
		return reinterpret_cast<const char8_t*>(str);
	}
};

template<>
struct FromString<const char8_t*>
{
	static const char8_t* getFrom(const char* str, int)
	{
		return reinterpret_cast<const char8_t*>(str);
	}
};
//====================================================================

class SimpleSerialization
{
	public:
		SimpleSerialization()=default;
		virtual ~SimpleSerialization()=default;

		template<typename S, typename T>
		void ToString(S s, T t)
		{
			m_stringData.append(s);
			m_stringData.append(SEPARATOR);
			m_stringData.append(::ToString<T>::toStr(t));
		}

		template<typename S, typename T, typename... Args>
		void ToString(S s, T t, Args... args)
		{
			m_stringData.append(s);
			m_stringData.append(SEPARATOR);
			m_stringData.append(::ToString<T>::toStr(t));
			m_stringData.append(SEPARATOR);
			ToString(args...);
		}

		void dump(std::ostream& outputStream)
		{
			outputStream<<m_stringData<<"\n";
		}

	private:
		std::string m_stringData;
};

//====================================================================

template<int N>
class SimpleUnserialization
{
	public:
		SimpleUnserialization(const char* data, const char* separator)
		:m_cstrSplit(data, separator)
		{}

		virtual ~SimpleUnserialization()=default;

		template<typename T>
		T get(const char* key)
		{
			std::optional<T> value=getValue<T>(key);
			if(value){
				return *value;
			}
			
			std::string excp="Key: ";
			excp+=key;
			excp+=" not foundy.";
			throw excp.c_str();
		}

		template<typename T>
		T get(const char* key, T&& defaultValue)
		{
			std::optional<T> value=getValue<T>(key);
			if(value){
				return *value;
			}
			return std::forward<T>(defaultValue);
		}

	private:
		CstrSplit<2*N> m_cstrSplit;

		template<typename T>
		std::optional<T> getValue(const char* key)
		{
			size_t keySize=std::strlen(key);
			for(size_t i=0; i<m_cstrSplit.dataSize(); i+=2){
				if(std::memcmp(m_cstrSplit[i], key, keySize*sizeof(char))==0){
					return FromString<T>::getFrom(m_cstrSplit[i+1], m_cstrSplit.chunkSize(i+1));
				}
			}
			return std::nullopt;
		}
};

//====================================================================

#endif
