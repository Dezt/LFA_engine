#include "TextManager.h"
#include "LFAAssert.h"

#include "QuadSet.h"
#include "DrawUtils.h"
#include "TextUtils.h"
#include "LFAMath.h"



/*
 *  textMgr.cpp
 *  watermelonHead
 *
 *  Created by Jorge Hernandez on 9/18/09.
 *  Copyright 2009 The Lo-Fi Apocalypse, Unc. All rights reserved.
 *
 
 */


using namespace LFA;
using namespace std;

LFA::textColor::textColor(float r, float g, float b, float a )
{
	cTop.r = cBottom.r = r;
	cTop.g = cBottom.g = g;
	cTop.b = cBottom.b = b;
	cTop.a = cBottom.a = a;
}
LFA::textColor::~textColor()
{}

#include<iostream>
std::wstring LFA::parseGlyphsTextFile( std::string const &pathToGlyphsFile )
{
	std::string line;
	ifstream myfile (pathToGlyphsFile.c_str());
	if (myfile.is_open())
	{
		while ( myfile.good() )
		{
			getline (myfile,line);
			if (line[0] != '#')
			{
				myfile.close();
				return LFA::TextUtils::stringToWString(line);
			}
		}
		myfile.close();
	}

	std::string errorStr = "[LFA::TextManager] Unable to open or parse file: " + pathToGlyphsFile ;
	printf(errorStr.c_str()); 
	LFA_Assert(false);

	return L"";
}


cTextManager::cTextManager(std::string pathToFontImageFile
						 , int windowWidth, int windowHeight
						 , unsigned int _textHeight
						 , textColor color
						 , std::wstring glyphList
						 , bool hasTextImage
						 ):
textHeight(_textHeight),
mColor(color),
// a default glyph list is provided here.
glyphs(glyphList == L"" ? L" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+=!?.,:;" : glyphList)
{		
	Image Img;        // Image loader
	if(Img.Load(pathToFontImageFile.c_str())!=Image::IMG_OK)
	{
		printf("TEXTMGR: could not load font TGA file!");
		LFA_Assert(0 && "TEXTMGR: could not load font TGA file!");
		exit(1);
	}

//	int r = (int)(textColor.r*255.0);
//	int g = (int)(textColor.g*255.0);
//	int b = (int)(textColor.b*255.0);


	//gather data from the RGB
	//these will hold all blue and red pixel positions from the top of the font.tga image to tell us how to space the glyphs.
	std::vector<int> red;
	std::vector<int> blue;
	//prepare to collect the glyph dimensions by reading the red and blue along the top of the font .tga
	IntCoords glyphRect;
	glyphRect.x = 0;
	glyphRect.y = 0;
	glyphRect.height = textHeight;  //don't use the actual image height as it may not correspond.
	
	red.push_back(0);// 0 should be the start x of the first glyph.
	blue.push_back(0);
	unsigned short int depth = Img.GetBitsPerPixel()/8;  //if 24 will yield 3 depth, if 32 will yield 4 depth.
	unsigned int x;
	unsigned int valueCount = Img.GetWidth() * depth;  //we are gonna traverse through every value along Y = 0 
	unsigned char * imageData = Img.GetImg();
	for (x = 0; x < valueCount; x++)
	{
		if (imageData[x] == 255){red.push_back(x/depth);}  //R
		x++;
		//if (imageData[x] == 255){green.push_back(x/depth);} //G  //green is not used to delineate anything atm
		x++;
		if (imageData[x] == 255){blue.push_back(x/depth);} //B
		
		if (depth == 4){ x++;}  //A
	}
	
	//assemble the glyph data
	std::vector<int>::iterator rItr; rItr = red.begin();
	std::vector<int>::iterator bItr; bItr = blue.begin();
	int lastRed = 0;
	int lastBlue = 0;
	++rItr; //offset the red itr since the width for glyph x reads at the same interval as the offset for glyph x - 1 (red = width,  blue = offset)
	for (; rItr != red.end(); ++rItr )
	{
		glyphRect.x = lastRed;
		glyphRect.width = (*rItr) - lastRed; //drawing width
		
		glyphOffset.push_back(lastRed - (*bItr));  //the difference between a glyphs drawing position and its offset = its offset!
		glyphRects.push_back(glyphRect);
		lastRed = (*rItr);
		lastBlue = (*bItr);
		++bItr;
		glyphWidth.push_back((*bItr) - lastBlue); //spacing width
	}
	
	
	if (!hasTextImage)
	{
		//now that the data is collected,  lets fill in the texture with white.  So then it will react properly when modulated with the vertex color.
		valueCount = Img.GetWidth() * Img.GetHeight() * depth;
		for (x = 0; x < valueCount; x++)
		{
			imageData[x] = 255; //R
			x++;
			imageData[x] = 255; //G
			x++;
			imageData[x] = 255; //B
			if (depth == 4){ x++;}  //A  - dont touch!
		}
	}

	
	IntCoords tempRect;
	tempRect.x = 0;
	tempRect.y = 0;
	tempRect.width = Img.GetWidth();
    tempRect.height = Img.GetHeight();	

	mQuadSet = new QuadSet(windowWidth,windowHeight);
	mQuadSet->setTextureImage(Img);
	mQuadSet->setTextureFiltering(FILTERING_LINEAR);

}

cTextManager::~cTextManager()
{
	delete mQuadSet;
}

std::vector<std::wstring> cTextManager::breakText(std::wstring text, unsigned int wrapLimit) 
{
	//arrange the text based on wrap limit. Each line will be an element inside outputLines.
	std::vector<std::wstring> outputLines;
	if (wrapLimit == 0) //wrapLimit = 0 means no wrap
		outputLines = TextUtils::tokenize(text, L"\n"); //just push the single line back,  taking forced newlines into account.
	else
	{	
		std::vector<std::wstring> words = TextUtils::tokenize(text, L" ");
		std::wstring currentLine;
		std::wstring tryLine;
		unsigned int wordNum = 0;
		 //textLength = 0;
		std::vector<std::wstring>::iterator wItr; 
		for (wItr = words.begin() ; wItr != words.end(); ++wItr) 
		{
			
			//test for newline characters.  If it is there, force a newline here.
			std::wstringstream ammendedString; 
			bool newlines = false;
			for (unsigned int i = 0; i < currentLine.size(); i++)
			{
				if (currentLine[i] != 0x000a)
					ammendedString << currentLine[i];
				else
				{
					outputLines.push_back(ammendedString.str());
					ammendedString.str(L""); //clear the stream
					newlines = true;
				}
			}
			if (newlines)
				currentLine = tryLine = ammendedString.str();

			tryLine.append((*wItr));
			unsigned int textLength = getTextLength(tryLine);

			if (textLength > wrapLimit) //did we reach the end of a line?
			{
				if (!wordNum) //if this is only the first word... and we still don't fit.  just force it in.
				{
					outputLines.push_back(tryLine);
					tryLine.clear();
				}
				else 
				{
					outputLines.push_back(currentLine);
					tryLine.clear();
					tryLine.append( (*wItr) + L" "); //don't lose the current word just because it didn't fit.
				}
				currentLine.clear();
				wordNum = 0;
			}
			else
			{
				currentLine = tryLine;
				currentLine.append(L" ");
				tryLine.append(L" ");
				wordNum++;
			}
		}
		outputLines.push_back(tryLine);
	}	
	
	return outputLines;
	
}

void cTextManager::write(std::wstring text, int x, int y, unsigned int wrapLimit,  textAlignment alignment, float scaleFactor)
{
	
	vector<wstring> splitText = breakText(text, wrapLimit);
	if (alignment == ALIGN_CENTER)
	{
		int offset = 0;
		int line = 0;
		std::vector<wstring>::iterator lineItr; 
		for (lineItr = splitText.begin(); lineItr != splitText.end(); ++lineItr) 
		{
			offset = int((getTextLength((*lineItr))/2) * scaleFactor);
			write((*lineItr), x-offset, y + (textHeight*line), 0,ALIGN_LEFT, scaleFactor);
			line++;
		}
	}
	if (alignment == ALIGN_RIGHT)
	{
		int offset = 0;
		int line = 0;
		std::vector<wstring>::iterator lineItr; 
		for (lineItr = splitText.begin(); lineItr != splitText.end(); ++lineItr) 
		{
			offset = getTextLength((*lineItr));
			write((*lineItr), x-offset, y + (textHeight*line), 0,ALIGN_LEFT, scaleFactor);
			line++;
		}
	}
	else if (alignment == ALIGN_LEFT)
		write(splitText, x, y, x, y, scaleFactor);
}


void cTextManager::write(std::vector<std::wstring> text, int x, int y, int pivotX, int pivotY, float scaleFactor)  //each vector element is a line. Defaults to Left aligned
{
	//TODO determine drawing position based on alignment.   LEFT CENTER RIGHT
//	unsigned int length = getTextLength(text);

	mQuadSet->beginEdit();
	
	//draw the text
	unsigned int charIt = 0;
	unsigned int glyphIt = 0;
	IntCoords destRect;
	destRect.height = textHeight;
	int currentX = x;
	int currentY = y;
	std::wstring line;
	std::vector<wstring>::iterator lineItr; 
	float imagedims[2];
	mQuadSet->getTextureImageDims(imagedims);
	for (lineItr = text.begin(); lineItr != text.end(); ++lineItr) 
	{
		line = (*lineItr);
		destRect.y = currentY;
		for (charIt = 0 ; charIt < line.length(); charIt++)//for every char in the sentence
		{
			for ( glyphIt = 0; glyphIt < glyphs.length();) //for every possible glyph
			{
				if(line[charIt] == glyphs[glyphIt])  //if a glyph is not found it will simply skip it.
				{
					//draw a glyph
					destRect.x = currentX + glyphOffset[glyphIt];
					destRect.width = glyphRects[glyphIt].width;

					if (fabs(scaleFactor-1.0)>0.001) 
					{
						destRect.height = textHeight; //keep values from adding
						destRect.y = currentY; //keep values from adding
						Math::scaleRect(pivotX,pivotY,&destRect, scaleFactor);
					}

					Quad* quad = mQuadSet->createQuad();
					
					quad->setPixelScreenCoords(FloatCoords((float)destRect.x, (float)destRect.y, (float)destRect.width, (float)destRect.height));
					quad->setPixelTextureCoords(FloatCoords((float)glyphRects[glyphIt].x, (float)glyphRects[glyphIt].y, (float)glyphRects[glyphIt].width, (float)glyphRects[glyphIt].height));

					quad->setTLColor(mColor.cTop);
					quad->setTRColor(mColor.cTop);
					quad->setBLColor(mColor.cBottom);
					quad->setBRColor(mColor.cBottom);

					currentX += glyphWidth[glyphIt];

					glyphIt = 0;
					charIt++;
					//break;
				}	
				else if (line[charIt] == 0x000a) //allow for forced newlines 
				{
					currentX = x;
					currentY += textHeight;
					destRect.y = currentY;
					charIt++;
				}	
				else
					glyphIt++;
					
			}
		}
		currentX = x;
		currentY += textHeight;
	}
	mQuadSet->endEdit();
	//return text.size();
}

void cTextManager::clear(void)
{
	mQuadSet->beginEdit();
	mQuadSet->removeAllQuads();
	mQuadSet->endEdit();
}

void cTextManager::draw(void)
{
	DrawUtils::draw(mQuadSet, BLEND_ALPHA);
}


void cTextManager::setColor(const Color &color, bool applyToExisting)
{
	textColor c(color.r, color.g, color.b, color.a);
	setColor(c, applyToExisting);
}
void cTextManager::setColor(textColor &color, bool applyToExisting)
{
	mColor = color;

	if (!applyToExisting)
		return;

	mQuadSet->beginEdit();
	Quad * quad = NULL;
	unsigned int i = 0;
	for (; i < mQuadSet->getNumQuads(); i ++)
	{
		quad = mQuadSet->getQuad(i);
		quad->setTLColor(mColor.cTop);
		quad->setTRColor(mColor.cTop);
		quad->setBLColor(mColor.cBottom);
		quad->setBRColor(mColor.cBottom);
	}

	mQuadSet->endEdit();
}

textColor cTextManager::getColor(void) const
{
	return mColor;
}

unsigned int cTextManager::getTextLength(std::wstring &text)
{
	unsigned int length = 0;
	
	unsigned int textIt = 0;
	unsigned int glyphIt = 0;
	for ( ; textIt < text.length(); textIt++)
	{
		for (glyphIt = 0; glyphIt < glyphs.length(); glyphIt++)
		{
			if(text[textIt] == glyphs[glyphIt])
			{
				length += glyphWidth[glyphIt];
				break;
			}
		}		
	}
	return length;
}

void cTextManager::fixOpenCloseQuote(std::wstring &text, std::wstring openQuote)
{
	bool odd = 1;
	std::wstring::iterator itr;
	std::wstring quote = L"\"";
	for (itr = text.begin(); itr != text.end(); ++itr) 
	{
		if ((*itr) == quote[0])
		{
			if (odd)
			{
				(*itr) = openQuote[0];
				odd = false;
			}
			else
				odd = true;
		}
	}
}



void LFA::cTextManager::setTextFiltering( LFA::TextureFiltering f )
{
	mQuadSet->setTextureFiltering(f);
}

