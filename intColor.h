#pragma once
#ifndef _INT_COLOR_H
#define _INT_COLOR_H

#include "Color.h"

namespace LFA
{
	// a useful class for encapsulating 32 bit color, note that LFA Engine defaults to using true floating point color using LFA:Color
	class intColor
	{
	public:
		intColor(unsigned int _r, unsigned int _g, unsigned int _b, unsigned int _alpha = 255)
			: r(_r), g(_g), b(_b), a(_alpha)
		{
			
		}
		intColor()
		{

		}
		
		virtual ~intColor()
		{

		}

		intColor operator*(float val) const;

		//these are optimized for speed sorting inside a set,  if you want to compare brightnesses, these must be adjusted accordingly.
		bool operator> (const intColor &clr) const; 
		bool operator>= (const intColor &clr) const {if (*this == clr) return true; return (*this > clr);}		
		bool operator< (const intColor &clr) const;
		bool operator<= (const intColor &clr) const {if (*this == clr) return true; return (*this < clr);}		

		bool operator== (const intColor &clr) const;
		bool operator!= (const intColor &clr) const {return !(*this == clr);}
		intColor & operator=(const intColor &clr);
		intColor & operator=(const LFA::Color &clr);

		LFA::Color asTrueColor(void);

		unsigned int operator[](unsigned int i)const;

		static const intColor Red;
		static const intColor Green;
		static const intColor Blue;
		static const intColor Yellow;
		static const intColor White;
		static const intColor Black;
		static const intColor Cyan;
		static const intColor Magenta;
		static const intColor Transparent;

		// intentionally public
		unsigned int r, g, b, a;
	private:
		
	};
}

#endif //_INT_COLOR_H