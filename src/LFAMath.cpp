#include "LFASTDHeaders.h"
#include "LFAMath.h"
#include "IntCoords.h"
#include "FloatCoords.h"

namespace LFA
{
	const float Math::PI = float( 4.0 * atan( 1.0 ) );
	const float Math::TWO_PI = float( 2.0 * PI );
	const float Math::PI_BY_2 = float( 0.5 * PI );

	float Math::toDegrees(float radians)
	{
		return (radians*180)/PI;

	}

	float Math::toRadians(float degrees)
	{
		return (degrees*PI)/180.0f;
	}


	void Math::makeIdentity(float mat[3][3])
	{
		mat[0][0]=1.0;mat[0][1]=0.0;mat[0][2]=0.0;
		mat[1][0]=0.0;mat[1][1]=1.0;mat[1][2]=0.0;
		mat[2][0]=0.0;mat[2][1]=0.0;mat[2][2]=1.0;
	}

	void Math::mult(float result[3][3], const float mat1[3][3], const float mat2[3][3])
	{
		for (unsigned int iRow = 0; iRow < 3; iRow++)
		{
			for (unsigned int iCol = 0; iCol < 3; iCol++)
			{
				result[iRow][iCol] =
					mat1[iRow][0]*mat2[0][iCol] +
					mat1[iRow][1]*mat2[1][iCol] +
					mat1[iRow][2]*mat2[2][iCol];
			}
		}

	}

	void Math::mult(float result[3], const float mat[3][3], const float vec[3])
	{
		for (unsigned int iRow = 0; iRow < 3; iRow++)
		{
			result[iRow] =
				mat[iRow][0]*vec[0] +
				mat[iRow][1]*vec[1] +
				mat[iRow][2]*vec[2];
		}
	}

	// [0,max] -> [0,1]
	void Math::normalize(float vals[2], const float dims[2])
	{
		for(unsigned int i = 0; i < 2; ++i)
		{
			vals[i] = vals[i]/dims[i];
		}

	}

	void Math::unnormalize(float vals[2], const float dims[2])
	{
		for(unsigned int i = 0; i < 2; ++i)
		{
			vals[i] = vals[i]*dims[i];
		}
	}

	// [0,1] -> [-1,1]
	void Math::expand(float vals[2])
	{
		for(unsigned int i = 0; i < 2; ++i)
		{	
			vals[i] = vals[i]*2.0f-1.0f;
		}
	}

	void Math::compress(float vals[2])
	{
		for(unsigned int i = 0; i < 2; ++i)
		{	
			vals[i] = (vals[i] + 1.0f)*0.5f;
		}
	}

	int Math::randomInt(int min, int max)
	{
		return min + rand() % (max - min + 1);
	}

	float Math::randomFloat(float min, float max)
	{
		return rand() / (float) RAND_MAX *(max - min) + min;
	}

	void Math::scaleRect( int pivotX, int pivotY, IntCoords *rect, float scaleFactor )
	{
		FloatCoords temp;
		temp.height = (float)rect->height;
		temp.width = (float)rect->width;
		temp.x = (float)rect->x;
		temp.y = (float)rect->y;
		scaleRect(pivotX,pivotY, &temp, scaleFactor);

		rect->height = (int)temp.height;
		rect->width = (int)temp.width;
		rect->x = (int)temp.x;
		rect->y = (int)temp.y;
	}

	void Math::scaleRect(int pivotX, int pivotY, FloatCoords *rect, float scaleFactor)
	{	
		//float diffX = pivotX - rect->x ;
		//float diffY = pivotY - rect->y ;
		float diffX = 0;
		float diffY = 0;
		if (rect->x > pivotX)
			diffX = rect->x - pivotX;
		else
			diffX = pivotX - rect->x;

		if (rect->y > pivotY)
			diffY = rect->y - pivotY;
		else
			diffY = pivotY - rect->y;

		float distance = sqrt((diffX*diffX) + (diffY*diffY)) ; //hypotenuse
		float targetDistance = distance * scaleFactor;
		float angle = 0; 
		if (distance) 
			angle = acos(diffX / distance);

		if (rect->x > pivotX)
			rect->x = pivotX + (cos(angle) * targetDistance);
		else
			rect->x = pivotX - (cos(angle) * targetDistance);

		if (rect->y > pivotY)
			rect->y = pivotY + (sin(angle) * targetDistance);
		else
			rect->y = pivotY - (sin(angle) * targetDistance);
		rect->width = rect->width * scaleFactor;
		rect->height = rect->height * scaleFactor;
	}

	
	int Math::floatToInt(float x)
	{
		int i = (int)x;
		float remainder = x - i;
		if (remainder > .5)
			return i + 1;
		else
			return i;
	}

	void Math::pixelsToScreenCoords(float coords[2], const float dims[2])
	{
		// 0 to 1
		normalize(coords, dims);
		// -1 to 1
		expand(coords);
	}

	void Math::pixelsToTexCoords(float coords[2], const float dims[2])
	{
		// 0 to 1
		normalize(coords, dims);
	}

	void Math::screenToPixelCoords(float coords[2], const float dims[2])
	{
		compress(coords);
		unnormalize(coords, dims);
	}

	void Math::texToPixelCoords(float coords[2], const float dims[2])
	{
		unnormalize(coords, dims);
	}

	FloatCoords Math::toFloatCoords(const IntCoords& coords, const float imagedims[2])
	{
		FloatCoords fCoords;
		float scratch[2];

		scratch[0] = (float)coords.x;
		scratch[1] = (float)coords.y;
		pixelsToTexCoords(scratch, imagedims);
		fCoords.x = scratch[0];
		fCoords.y = scratch[1];

		scratch[0] = (float)coords.width;
		scratch[1] = (float)coords.height;
		pixelsToTexCoords(scratch, imagedims);
		fCoords.width = scratch[0];
		fCoords.height = scratch[1];

		return fCoords;
	}

	FloatCoords Math::toFloatCoords(const FloatCoords& coords, const float imagedims[2])
	{
		FloatCoords fCoords;
		float scratch[2];

		scratch[0] = coords.x;
		scratch[1] = coords.y;
		pixelsToTexCoords(scratch, imagedims);
		fCoords.x = scratch[0];
		fCoords.y = scratch[1];

		scratch[0] = coords.width;
		scratch[1] = coords.height;
		pixelsToTexCoords(scratch, imagedims);
		fCoords.width = scratch[0];
		fCoords.height = scratch[1];

		return fCoords;
	}

	float Math::getDistance( float x1, float y1, float x2, float y2 )
	{
		float xDiff = x1 - x2;
		float yDiff = y1 - y2;
		xDiff *= xDiff;
		yDiff *= yDiff;

		return sqrt(xDiff + yDiff);
	}

	float Math::getDistance( float x1, float y1, float z1, float x2, float y2, float z2 )
	{
		float xDiff = x1 - x2;
		float yDiff = y1 - y2;
		float zDiff = z1 - z2;
		xDiff *= xDiff;
		yDiff *= yDiff;
		zDiff *= zDiff;

		return sqrt(xDiff + yDiff + zDiff);
	}

	float Math::getAngle( float x1, float y1, float x2, float y2 )
	{
		return atan2(x1-x2,y1-y2) ;
	}

	float Math::getDifference(float x1, float x2 )
	{
		if (x1 > x2)
			return x1 - x2;
		else if (x1 < x2)
			return x2 - x1;
		return 0;
	}

	int Math::getDifference( int x1, int x2 )
	{
		if (x1 > x2)
			return x1 - x2;
		else if (x1 < x2)
			return x2 - x1;
		return 0;
	}

	void Math::plotLine( int x1, int y1, int x2, int y2, std::vector<std::pair<int,int> > &coords )
	{
		//Bresenham's Line Algorithm taken from:   http://roguebasin.roguelikedevelopment.org/index.php?title=Bresenham's_Line_Algorithm

		coords.clear();

		std::pair<int,int> coord;

		signed char ix;
		signed char iy;

		// if x1 == x2 or y1 == y2, then it does not matter what we set here
		int delta_x = (x2 > x1?(ix = 1, x2 - x1):(ix = -1, x1 - x2)) << 1;
		int delta_y = (y2 > y1?(iy = 1, y2 - y1):(iy = -1, y1 - y2)) << 1;

		coord.first = x1;
		coord.second = y1;
		coords.push_back(coord);

		if (delta_x >= delta_y)
		{
			// error may go below zero
			int error = delta_y - (delta_x >> 1);

			while (x1 != x2)
			{
				if (error >= 0)
				{
					if (error || (ix > 0))
					{
						y1 += iy;
						error -= delta_x;
					}
					// else do nothing
				}
				// else do nothing

				x1 += ix;
				error += delta_y;

				coord.first = x1;
				coord.second = y1;
				coords.push_back(coord);
			}
		}
		else
		{
			// error may go below zero
			int error = delta_x - (delta_y >> 1);

			while (y1 != y2)
			{
				if (error >= 0)
				{
					if (error || (iy > 0))
					{
						x1 += ix;
						error -= delta_y;
					}
					// else do nothing
				}
				// else do nothing

				y1 += iy;
				error += delta_x;

				coord.first = x1;
				coord.second = y1;
				coords.push_back(coord);
			}
		}
	}

	double Math::round( double x , const int precisionInt /*= 1000*/ ) /*1000 for accuracy to 3 decimal places */
	{
		return int(x*precisionInt + (x<0? -0.5 : 0.5))/(double)precisionInt;
	}
}