#pragma once
#ifndef COLOR_H
#define COLOR_H

namespace LFA
{
	class intColor;

	// class for encapsulating floating point Color  from 0.0f  - 1.0f
	class Color
	{
	public:
		Color(float _r, float _g, float _b, float _alpha = 1.0f)
			: r(_r), g(_g), b(_b), a(_alpha)
		{
			
		}
		Color()
		{

		}
		
		virtual ~Color()
		{

		}

		Color operator*(float val) const;

		bool operator== (const Color &clr) const;
		bool operator!= (const Color &clr) const {return !(*this == clr);}
		Color & operator=(const Color &clr);
		Color & operator=(const LFA::intColor &clr);

		float operator[](unsigned int i)const;

		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color White;
		static const Color Black;
		static const Color Cyan;
		static const Color Magenta;
		static const Color Grey;

		// intentionally public
		float r, g, b, a;
	private:
		
	};
}

#endif