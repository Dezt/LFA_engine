#pragma once
#ifndef LFA_DATA_FILE_READER_H
#define LFA_DATA_FILE_READER_H



#include <vector>
#include <string>
/*#if defined(__GNUC__) && !defined(ANDROID)
#include <ext/hash_map> // i guess hash_map is not standard yet...
#else
#include <hash_map>
#endif*/

//#include "LFAAssert.h"


//using namespace stdext;



namespace LFA
{

	class dataFileReader
	{
		public:
			void readFile(const std::string &file);
			dataFileReader(const char* dataFile, const bool fileIsDataFileList);
			~dataFileReader();

			std::string getData(const std::string  &name);

			void setData(const std::string &name, const std::string &val);
			void writeData(const char* dataFile, const char* comment = "");  //save our variables to a readable data file.
			const unsigned int size(void);

			/**
			tokenize a std::string into a std::vector like:
				tokenize(command, " ");
			*/
			void tokenize(const std::string& str, const std::string& delimiters, std::vector<std::string> &output);
			//std::vector<std::wstring> wtokenize(const std::wstring& str,const std::wstring& delimiters);

			/**
			* removes white space from a string
			*/
			void removeWhiteSpace (std::string &str);

			/**
			cast anything to a string for easy concatenation like:
				std::string s = ToString( "The next thing is a number " ) + ToString(123);
			*/
			template<class TYPE> std::string toString( const TYPE & t );

		private:
		//	hash_map <const char *, const char *, str_compare> gameVariables;
			std::vector<std::string>::iterator keyItr;
			std::vector<std::string>::iterator valueItr;
			std::vector<std::string> keys;
			std::vector<std::string> values;

	};

}





#endif
