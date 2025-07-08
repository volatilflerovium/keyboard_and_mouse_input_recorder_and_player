#ifndef PROFILER_H
#define PROFILER_H

#include <iostream>
#include <chrono>
#include <iomanip>

#include "time_traits.h"

//####################################################################

template<typename TM>
class Profiler
{
	public:
		Profiler()
		: m_total(0.0), m_count(0), m_isInitialized(false)
		{}

		~Profiler()=default;

		void start()
		{
			m_isInitialized=true;
			m_startPoint=std::chrono::high_resolution_clock::now();
		}

		void stop(bool doPrint=true);

		void totalTime(uint precision=3) const;
		
		void reset()
		{
			m_isInitialized=false;
			m_count=0;
			m_total=0;
		}

	private:
		std::chrono::system_clock::time_point m_startPoint;
		long long m_total;
		int m_count;
		bool m_isInitialized;
		
};

//--------------------------------------------------------------------

template<typename TM>
inline void Profiler<TM>::totalTime(uint precision) const
{
	//std::cout<<m_count<<"<<----------------"<< m_total<<"\n";
	//#ifdef DEBUG
	std::ios_base::fmtflags f( std::cout.flags() );
	std::cout<<"Average elapsed time: ";
	std::cout<<std::fixed<<std::setprecision(precision)<<(1.0*m_total)/(1.0*m_count)<<TimeType<TM>::timeUnit<<std::endl;
	std::cout.flags( f );
	//#endif
}

//--------------------------------------------------------------------

template<typename TM>
void Profiler<TM>::stop(bool doPrint){
	if(m_isInitialized){
		auto elapsed = std::chrono::high_resolution_clock::now() - m_startPoint;

		double elep = (std::chrono::duration_cast<TM>(elapsed).count());
		//long long elep = (std::chrono::duration_cast<TM>(elapsed).count());
		
		m_total+=elep;
		m_count++;
		if(doPrint){
			//#ifdef DEBUG
			std::cout<<"Elapsed time:"<<1.0*elep<<" "<<TimeType<TM>::timeUnit<<"\n";
			//#endif
		}
	}
	else{
		std::cout<<"Timer did not start."<<'\n';
	}
	m_isInitialized=false;
}

//====================================================================

#endif
