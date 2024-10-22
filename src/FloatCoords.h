#pragma once
#ifndef FLOATCOORDS_H
#define FLOATCOORDS_H

namespace LFA
{
	// useful class for encapsulating pixel/texel coordinates
	class FloatCoords
	{
	public:
		FloatCoords(float _x, float _y, float _width, float _height)
			: x(_x), y(_y), width(_width), height(_height)
		{
			
		}
		FloatCoords(){}
		
		virtual~FloatCoords()
		{

		}

		// intentionally public
		float x, y, width, height;
	private:
		
	};
}

#endif