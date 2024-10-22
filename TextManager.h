#pragma once
#ifndef LFA_TEXT_MGR_H
#define LFA_TEXT_MGR_H


/*
 *  faceMgr.h
 *  watermelonHead
 *
 *  Created by Jorge Hernandez on 9/17/09.
 *  Copyright 2009 The Lo-Fi Apocalypse, Inc. All rights reserved.
 *
 this class relies that 
 - RED pixels along the top of the font image deliniating the DRAWING widths of each glpyh (pixel inclusive)
 - BLUE pixels along the top of the font image delinating the text POSITIONING of each glyph relative to a previous glyph
 (magenta would delinate same position for both)

 this class is meant to be instanced once for each desired font variant.
 */

#include "TextUtils.h"
#include "Image.h"
#include "IntCoords.h"
#include "Color.h"
#include "TextureFiltering.h"

namespace LFA
{

	class textColor
	{
	public:
		textColor(float r, float g, float b, float a = 1.0f);
		~textColor();

		LFA::Color cTop;    //these colors are applied to text accordingly so you can have gradients in your text.
		LFA::Color cBottom;
	};
	enum textAlignment
	{
		ALIGN_LEFT = 0,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};


	/*
	utility so you can still declare a cTextManager as const in a constructor and also still read the glyphs from an input file.
	note that on multiplatform projects,  having the glyphs declared inline can cause problems with unusual characters if different IDE's read the cpp files
	using different character encodings (unicode, iso... etc.)  So its best to use a datafile to declare glyphs.
	*/
	std::wstring parseGlyphsTextFile(std::string const &pathToGlyphsFile); 


	class QuadSet;
	class cTextManager
		{
		public:

			cTextManager(std::string pathToFontImageFile
				, int windowWidth, int windowHeight
				, unsigned int textHeight
				, textColor color
				, std::wstring glyphList = L""
				, bool hasTextImage = false);  //hasTextImage tells the textMgr whether you want to use the RGB values in the TGA (true) or if they are just magenta lines delineating  glyphs (false)
			~cTextManager();

			//will split the text into lines based on pixel wrap limit. 
			std::vector<std::wstring> breakText(std::wstring text, unsigned int wrapLimit); 
		
			//note that the 2nd override assumes LEFT_ALIGN.  the reason is that breakText  parses out and processes newline characters into the wstring array
			//it would be  harder to set alignment and also parse through a particular line for formatting characters in the 2nd override
			//so lets just keep the code clean and only allow alignment on the override that runs breakText
			void write(std::wstring text, int x, int y, unsigned int wrapLimit, textAlignment alignment = ALIGN_LEFT, float scaleFactor = 1.0f);  //0 wrap means no wrapping.
			void write(std::vector<std::wstring> text, int x, int y, int pivotX, int pivotY, float scaleFactor) ;  //each vector element is a line.

			void clear(void);

			void draw(void);

			void setColor(const Color &color, bool applyToExisting = true);
			void setColor(textColor &color, bool applyToExisting = true);

			textColor getColor(void) const;
			int getTextHeight(void) {return textHeight;}

			unsigned int getTextLength(std::wstring &text);
			void fixOpenCloseQuote(std::wstring &text, std::wstring openQuote);  //will replace every odd numbered " instance with the first character in openQuote
			
			void setTextFiltering(LFA::TextureFiltering f);
		private:
			
			const std::wstring glyphs;
			std::vector<IntCoords> glyphRects;
			std::vector<int> glyphOffset;
			std::vector<int> glyphWidth;
			
			std::wstring::iterator strItr;
			const int textHeight;

			QuadSet* mQuadSet;

			textColor mColor;

		};
}


#endif //LFA_TEXT_MGR_H
