/*********************************************************************
* template<> class dbg                                               *
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _DEBUG_UTILS_H
#define _DEBUG_UTILS_H

#include <iostream>

//====================================================================

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.

#define RESET   "\033[0m"
#define BLACK   "\033[30m"              /* Black */
#define RED     "\033[31m"              /* Red */
#define GREEN   "\033[32m"              /* Green */
#define YELLOW  "\033[33m"              /* Yellow */
#define BLUE    "\033[34m"              /* Blue */
#define MAGENTA "\033[35m"              /* Magenta */
#define CYAN    "\033[36m"              /* Cyan */
#define WHITE   "\033[37m"              /* White */
#define BOLDBLACK   "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"   /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"   /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"   /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"   /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"   /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"   /* Bold White */

//====================================================================

template<typename T>
void dbg(T t)
{
	#ifdef DEBUG
	std::cout<<t<<"\n";
	#endif
}

template<typename T, typename... Args>
void dbg(T t, Args... args)
{
	#ifdef DEBUG
	std::cout<<t;
	dbg(args...);
	#endif
}

//====================================================================

template<typename... Args>
void debugWarning(Args... args)
{
	#ifdef DEBUG
	std::cout<<BOLDYELLOW;
	dbg(args...);
	std::cout<<RESET;
	#endif
}

//====================================================================

#endif
