#ifndef TIME_TRAITS_H
#define TIME_TRAITS_H

#include <chrono>

//====================================================================

template<typename T>
struct TimeType
{
	enum { val = false };
};

template<>
struct TimeType<std::chrono::seconds>
{
	enum { 
		val = true, 
	};
	//static constexpr double conversionFactor=1.0;
	static constexpr double conversionFactor=1.0;
	static constexpr const char* timeUnit="secs";
};

template<>
struct TimeType<std::chrono::milliseconds>
{
	enum { 
		val = true,
	};
	//static constexpr double conversionFactor=1.0/1000.0;
	static constexpr double conversionFactor=1000.0;
	static constexpr const char* timeUnit="ms";
};

template<>
struct TimeType<std::chrono::microseconds>
{
	enum { 
		val = true,
	};
	//static constexpr double conversionFactor=1.0/1000000.0;
	static constexpr double conversionFactor=1000000.0;
	static constexpr const char* timeUnit="μs";
};

template<>
struct TimeType<std::chrono::nanoseconds>
{
	enum { 
		val = true,
	};
	//static constexpr double conversionFactor=1.0/1000000000.0;
	static constexpr double conversionFactor=1000000000.0;
	static constexpr const char* timeUnit="ns";
};

//====================================================================

#endif
