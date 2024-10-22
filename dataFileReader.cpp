#include "dataFileReader.h"
//#include "LFASTDHeaders.h"
//#include "lfalog.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace LFA;


dataFileReader :: dataFileReader(const char* dataFile, const bool fileIsDataFileList) //grab all variables from adatafiles
{
	readFile(dataFile);

	if (fileIsDataFileList)  //if true, this means that this is a list of other data files,  and we should go into them and parse them too.
	{

		int dataFileNum = atoi(getData("dataFileNum").c_str());	

		for (int x=0; x < dataFileNum ; x++) //grab variables from each of the files annotated in the 'central' dataFile
		{
			stringstream ss;
			ss << "file" << x;

			std::string fileName = getData(ss.str());
			if (fileName.size())
				readFile(fileName);
		}
	}
}

dataFileReader :: ~dataFileReader()
{
}



void dataFileReader :: readFile(const std::string &file)
{
	ifstream fin;

	fin.open(file.c_str()); // default, open for reading

	if ( !fin.is_open() ) //file could not be opened
	{
		std::cout <<"[dataFileReader] could not open data file: " << file << "\n";
		return;
	}
	else
	{
		std::cout << "[dataFileReader] Reading data file: " << file << "\n";
	}


	std::string line;
	std::vector<std::string> strTokens;
	std::string varName;
	std::string varVal;

	 while( getline( fin, line ) )
	 {
		// textFile.push_back( line );
		if (line.size() && line.find_first_of("#") != 0) //if # is the first character, it is a comment.
		{
			strTokens.clear();
			tokenize(line,"=", strTokens);

			varName = strTokens[0];
			removeWhiteSpace(varName);

			if (strTokens.size() == 1)
				varVal.clear();    //if there is no second entry,  make sure that it is at least defined.
			else
			{
				varVal = strTokens[1];
				removeWhiteSpace(varVal);
			}

			setData(varName,varVal);
		}
	 }

	fin.close();
}


std::string dataFileReader :: getData(const std::string &name)
{
	//if (gameVariables[name] != gameVariables.end())
	//	return gameVariables[name];
	keyItr = keys.begin();
	valueItr = values.begin();
	for (; keyItr != keys.end();)
	{
		if ((*keyItr).c_str() == name)
		{
			return (*valueItr);  //keep track of positives
		}
		keyItr++;
		valueItr++;
	}
	return "";
}

void dataFileReader :: setData(const std::string &name, const std::string &val)
{
	keyItr = keys.begin();
	valueItr = values.begin();
	for (; keyItr != keys.end();) //don't allow duplicate keys.
	{
		if ((*keyItr) == name)
		{
			(*valueItr) = val;
			return;
		}		
		keyItr++;
		valueItr++;
	}
	keys.push_back(name);
	values.push_back(val);
	//gameVariables[name] = value;
}

void dataFileReader :: writeData(const char* dataFile, const char * comment)
{
	ofstream file;
	file.open (dataFile);  

	if (comment != "")
	{
		file << "#"<< comment <<"\n";
	}

	keyItr = keys.begin();
	valueItr = values.begin();
	for( ; keyItr != keys.end(); ) 
	{
		 file << (*keyItr).c_str() << "=" << (*valueItr).c_str() << "\n";

		keyItr++;
		valueItr++;
	}

  file.close();
}

const unsigned int dataFileReader ::size(void)
{
	return (unsigned int)keys.size();
}



template<class TYPE> std::string dataFileReader::toString( const TYPE & t ) 
{
	std::ostringstream os;
	os << t;
	return os.str();
}

void dataFileReader::tokenize(const std::string& str,const std::string& delimiters, std::vector<std::string> &output)
{
	// skip delimiters at beginning.
    	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    	
	// find first "non-delimiter".
    	string::size_type pos = str.find_first_of(delimiters, lastPos);

    	while (string::npos != pos || string::npos != lastPos)
    	{
        	// found a token, add it to the vector.
        	output.push_back(str.substr(lastPos, pos - lastPos));
		
        	// skip delimiters.  Note the "not_of"
        	lastPos = str.find_first_not_of(delimiters, pos);
		
        	// find next "non-delimiter"
        	pos = str.find_first_of(delimiters, lastPos);
    	}
}


void dataFileReader::removeWhiteSpace (std::string &str)
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
