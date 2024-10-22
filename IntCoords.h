#pragma once
#ifndef INTCOORDS_H
#define INTCOORDS_H

namespace LFA
{
	// useful class for encapsulating pixel/texel coordinates
	class IntCoords
	{
	public:
		IntCoords(int _x, int _y, int _width, int _height)
			: x(_x), y(_y), width(_width), height(_height)
		{
			
		}
		IntCoords(){}
		
		virtual~IntCoords()
		{

		}

		// intentionally public
		int x, y, width, height;
	private:
		
	};
}

#endif