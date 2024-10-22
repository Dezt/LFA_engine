/*
 *	common.c
 *	written by Holmes Futrell
 *	use however you want
 */
#include "TextUtils.h"

#include <fstream>  //needed for fileToString

using namespace std;

namespace LFA
{
	std::string TextUtils::toString(int number)
	{
		std::stringstream ss;
		ss << number;
		return ss.str();
	}

	std::wstring TextUtils::toWString(int number)
	{
		std::wstringstream converter;
		std::wstring wstr;
		converter << number;
		converter >> wstr;
		return wstr;
	}


	std::vector<std::string> TextUtils::tokenize(const string& str,const string& delimiters)
	{
		vector<string> tokens;

		// skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);

		// find first "non-delimiter".
		string::size_type pos = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			// found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));

			// skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}

		return tokens;
	}

	std::vector<std::wstring> TextUtils::tokenize(const std::wstring& str,const std::wstring& delimiters)
	{
		vector<wstring> tokens;

		// skip delimiters at beginning.
		wstring::size_type lastPos = str.find_first_not_of(delimiters, 0);

		// find first "non-delimiter".
		wstring::size_type pos = str.find_first_of(delimiters, lastPos);

		while (wstring::npos != pos || wstring::npos != lastPos)
		{
			// found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));

			// skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}

		return tokens;	
	}


	void TextUtils::removeWhiteSpace (std::string &str)
	{
		std::string temp;

		//trim beginning white space
		while( str.find_first_of(" ") == 0 && str.size())
		{	
			std::string::iterator it;
			it = str.begin();
			str.erase(it);
		}

		//trim end white space
		while( str.find_last_of(" ") == str.length()-1  && str.size())
		{
			str.erase(str.length()-1);
		}
	}

	std::string TextUtils::wstringToString( const std::wstring &ws )
	{
		std::string s;
		s.assign(ws.begin(), ws.end());
		return s;
	}

	std::wstring TextUtils::stringToWString( const std::string &s )
	{
		//std::wstring ws;
		//ws.assign(s.begin(), s.end());
		//return ws;
		std::wostringstream ss;
		ss << s.c_str();
		return ss.str();
	}

	int TextUtils::strToInt(const std::string &strConvert) 
	{ 
		int intReturn; 

		// NOTE: You should probably do some checks to ensure that 
		// this string contains only numbers. If the string is not 
		// a valid integer, zero will be returned. (use stringstream)
		intReturn = atoi(strConvert.c_str()); 

		return intReturn ; 
	}

	float TextUtils::strToFloat( const std::string &strConvert )
	{
		std::stringstream ss(strConvert);
		float n;
		ss >> n;
		return n;
	}
	
	std::string TextUtils::baseName(const std::string &fullPath)
	{
		if (fullPath == "")
			return "";
		std::vector<std::string> tokens = TextUtils::tokenize(fullPath, "\\/"); //get the paths out.
		tokens = TextUtils::tokenize(tokens.back(), ".");
		
		return tokens[0];
	}
	
	bool TextUtils::fileToString( const std::string &filePath, std::string &data )
	{
		data.clear();

		string line;
		ifstream Myfile;
		Myfile.open (filePath.c_str());
		if(Myfile.is_open())
		{
			while(!Myfile.eof())
			{
				getline(Myfile,line);
				data += "\n";
				data += line;
			}
			Myfile.close();
			return true;
		}
		return false;
	}
	
	bool TextUtils::validateString(const std::string &val, const std::string &badCharString)
	{
		unsigned int valSize = val.size();
		unsigned int checkSize = badCharString.size();
		
		for (unsigned int x = 0; x < valSize; x++) 
		{
			for (unsigned int i = 0; i < checkSize; i++) 
			{
				if (val[x] == badCharString[i])
					return false;
			}
		}
		return true;
		
	}
	
	
	
	
} // namespace LFA


