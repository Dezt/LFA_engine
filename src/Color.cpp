#include "LFASTDHeaders.h"
#include "LFAAssert.h"
#include "Color.h"
#include "intColor.h"

namespace LFA
{
	const Color Color::Red = Color(1.0,0.0,0.0,1.0);
	const Color Color::Green = Color(0.0,1.0,0.0,1.0);
	const Color Color::Blue = Color(0.0,0.0,1.0,1.0);
	const Color Color::Yellow = Color(1.0,1.0,0.0,1.0);
	const Color Color::White = Color(1.0,1.0,1.0,1.0);
	const Color Color::Black = Color(0.0,0.0,0.0,1.0);
	const Color Color::Cyan = Color(0.0,1.0,1.0,1.0);
	const Color Color::Magenta = Color(1.0,0.0,1.0,1.0);
	const Color Color::Grey = Color(.5, .5, .5, 1.0);

	float Color::operator[](unsigned int i)const
	{
		if (i==0)
			return r;
		if (i==1)
			return g;
		if (i==2)
			return b;
		if (i==3)
			return a;
		LFA_Assert(false);
		return 0;
	}

	Color Color::operator*(float val) const
	{
		Color col = *this;
		col.r*=val;
		col.g*=val;
		col.b*=val;
		col.a*=val;
		return col;
	}

	bool Color::operator==( const Color &clr ) const
	{
		if((r == clr.r) &&
			(g == clr.g)&&
			(b == clr.b)&&
			(a == clr.a))
				return true;
		return false;
	}

	Color & Color::operator=( const Color &clr )
	{
		r = clr.r;
		g = clr.g;
		b = clr.b;
		a = clr.a;
		return *this;
	}

	Color & Color::operator=( const LFA::intColor &clr )
	{
		r = float(clr.r) /255.0f;
		g = float(clr.g) /255.0f;
		b = float(clr.b) /255.0f;
		a = float(clr.a) /255.0f;
		return *this;
	}
}