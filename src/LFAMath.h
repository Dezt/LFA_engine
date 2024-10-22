#pragma once
#ifndef LFA_MATH_H
#define LFA_MATH_H

#include <vector>

namespace LFA
{
	class IntCoords;
	class FloatCoords;
	class Math
	{
	public:
		static const float PI;
		static const float TWO_PI;
		static const float PI_BY_2;

		static float toDegrees(float radians);
		static float toRadians(float degrees);

		static void makeIdentity(float mat[3][3]);

		//result = mat1*mat2
		static void mult(float result[3][3], const float mat1[3][3], const float mat2[3][3]);

		// result = mat1*vec
		static void mult(float result[3], const float mat[3][3], float const vec[3]);

		// [0,max] -> [0,1]
		static void normalize(float vals[2], const float dims[2]);

		// [0,1] -> [0,max]
		static void unnormalize(float vals[2], const float dims[2]);

		// [0,1] -> [-1,1]
		static void expand(float vals[2]);

		// [-1,1] -> [0,1]
		static void compress(float vals[2]);

		/*
		Produces a random int x, min <= x <= max 
		following a uniform distribution 
		*/
		static int randomInt(int min, int max);

		/*
		Produces a random float x, min <= x <= max 
		following a uniform distribution
		*/
		static float randomFloat(float min, float max);

		/*
		Rounds to the nearest floating point, depending on the precision given
		for example, 1000 as precisionInt rounds to 3 decimal places;
		*/
		static double round( double x , const int precisionInt = 1000); 

		static void scaleRect(int pivotX, int pivotY, IntCoords *rect, float scaleFactor);
		static void scaleRect(int pivotX, int pivotY, FloatCoords *rect, float scaleFactor);

		static float getDistance(float x1, float y1, float z1, float x2, float y2, float z2); //order matters, this will return +/- distances if the points are reversed
		static float getDistance(float x1, float y1, float x2, float y2);
		static float getAngle(float x1, float y1, float x2, float y2);
		static float getDifference(float x1, float x2);
		static int getDifference(int x1, int x2);

		/*
		Bresenham's Line Algorithm.   (an ideal line represented over a raster grid.)
		returns the closest resemblance to an ideal line in the vector.
		*/
		static void plotLine(int x1, int y1, int x2, int y2, std::vector<std::pair<int,int> > &coords);   
		
		/*
		 will attempt to round off the float to the nearest integer instead of truncating.
		 */
		static int floatToInt(float x);

		static void pixelsToScreenCoords(float coords[2], const float dims[2]);
		static void pixelsToTexCoords(float coords[2], const float dims[2]);

		static void screenToPixelCoords(float coords[2], const float dims[2]);
		static void texToPixelCoords(float coords[2], const float dims[2]);
		

		/* Given pixel coordinates and image dimensions, computes the coords in texture space*/
		static FloatCoords toFloatCoords(const IntCoords& coords, const float imagedims[2]);
		static FloatCoords toFloatCoords(const FloatCoords& coords, const float imagedims[2]);
	};
}

#endif