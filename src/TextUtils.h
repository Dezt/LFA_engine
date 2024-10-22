#pragma once
#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include "LFASTDHeaders.h"

/*
 *	TextUtils.h
 *	written by Holmes Futrell
 *	use however you want
 */

namespace LFA
{
	class TextUtils
	{
	public:
		static std::string toString(int number);
		static std::wstring toWString(int number);
		static std::string wstringToString(const std::wstring &ws);
		static std::wstring stringToWString(const std::string &s);
		static std::vector<std::string> tokenize(const std::string& str,const std::string& delimiters);
		static std::vector<std::wstring> tokenize(const std::wstring& str,const std::wstring& delimiters);
		static void removeWhiteSpace (std::string &str);
		static int strToInt(const std::string &strConvert) ;
		static float strToFloat(const std::string &strConvert);
		static std::string baseName(const std::string &fullPath);
		static bool fileToString(const std::string &filePath, std::string &data); //this function is NOT intended for use with binary files.
		static bool validateString (const std::string &val, const std::string &badCharString);
	};
}

#endif



