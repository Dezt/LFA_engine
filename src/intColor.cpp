#include "LFASTDHeaders.h"
#include "LFAAssert.h"
#include "intColor.h"
#include "LFAMath.h"

namespace LFA
{
	const intColor intColor::Red = intColor(255, 0 , 0, 255);
	const intColor intColor::Green = intColor(0, 255, 0, 255);
	const intColor intColor::Blue = intColor(0, 0, 255, 255);
	const intColor intColor::Yellow = intColor(255, 255, 0, 255);
	const intColor intColor::White = intColor(255,255,255,255);
	const intColor intColor::Black = intColor(0, 0, 0, 255);
	const intColor intColor::Cyan = intColor(0,255,255,255);
	const intColor intColor::Magenta = intColor(255,0,255,255);
	const intColor intColor::Transparent = intColor(0, 0, 0, 0);

	unsigned int intColor::operator[](unsigned int i)const
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

	intColor intColor::operator*(float val) const
	{
		intColor col = *this;
		col.r = (unsigned int)((float)col.r  * val);
		col.g = (unsigned int)((float)col.g  * val);
		col.b = (unsigned int)((float)col.b  * val);
		col.a = (unsigned int)((float)col.a  * val);
		return col;
	}

	bool intColor::operator==( const intColor &clr ) const
	{
		if((r == clr.r) &&
			(g == clr.g)&&
			(b == clr.b)&&
			(a == clr.a))
				return true;
		return false;
	}

	intColor & intColor::operator=( const intColor &clr )
	{
		r = clr.r;
		g = clr.g;
		b = clr.b;
		a = clr.a;
		return *this;
	}

	intColor & intColor::operator=( const LFA::Color &clr )
	{
		r = (unsigned int)(clr.r *255.0f);
		g = (unsigned int)(clr.g *255.0f);
		b = (unsigned int)(clr.b *255.0f);
		a = (unsigned int)(clr.a *255.0f);
		return *this;
	}

	LFA::Color intColor::asTrueColor( void )
	{
		LFA::Color clr;
		clr.r = float(r)/255.0f;
		clr.g = float(g)/255.0f;
		clr.b = float(b)/255.0f;
		clr.a = float(a)/255.0f;
		return clr;
	}

	bool intColor::operator> ( const intColor &clr ) const
	{
		if (r == clr.r)
		{
			if (g == clr.g)
			{
				if (b == clr.b)
				{
					if (a == clr.a)
					{
						return false; //same as ==
					}
					else return (a > clr.a);
				}
				else return (b > clr.b);
			}
			else return (g > clr.g);
		}
		else return (r > clr.r);
	}

	bool intColor::operator< ( const intColor &clr ) const
	{
		if (r == clr.r)
		{
			if (g == clr.g)
			{
				if (b == clr.b)
				{
					if (a == clr.a)
					{
						return false; //same as ==
					}
					else return (a < clr.a);
				}
				else return (b < clr.b);
			}
			else return (g < clr.g);
		}
		else return (r < clr.r);
	}
}