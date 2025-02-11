/*********************************************************************
*         	                                                         *
* Version: 1.0                                                       *
* Date:    09-02-2025                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#ifndef _WX_STRING_ARRAY_H
#define _WX_STRING_ARRAY_H


#ifdef WX_STRING_ARRAY
	typedef wxArrayString ArrayStringType;
#else
	typedef std::vector<std::string> ArrayStringType;
#endif

#endif

