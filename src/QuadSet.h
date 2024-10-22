#pragma once
#ifndef QUADSET_H
#define QUADSET_H
#include "Config.h"

#include <list>
#include <string>

#include "LFAAssert.h"

#include "IntCoords.h"
#include "FloatCoords.h"
#include "Color.h"
#include "TextureFiltering.h"

namespace LFA 
{
	class Image;
	class Quad;

	class QuadSet
	{
	public:
		QuadSet(int windowWidth, int windowHeight);

		// Must call if window gets resized
		void windowResized(int windowWidth, int windowHeight);

		virtual ~QuadSet();

		// *Must* call before any modifications to individual quads can be made*/
		void beginEdit(void);
		// *Must* call after any modifications
		void endEdit(void);

		// appended at the end of the list*/
		Quad* createQuad(void);
		bool removeQuad(unsigned int i);
		bool removeQuad(Quad* q);

		/* This is an linear search operation.*/
		Quad* getQuad(unsigned int i) const;

		unsigned int getNumQuads(void) const
		{
			return mQuads.size();
		}

		void removeAllQuads(void);

		//setTextureImage returns true if the texture has alpha, otherwise false
		void setTextureImage(const Image& image); 
		void setTextureImage(std::string filePath);
		void clearTextureImage(void);
		bool hasTexture(void) const{ return mHasTexture; }
		bool hasTextureAlpha(void) const {if (mHasTexture && mHasTextureAlpha) return true; return false;}

		const GLfloat* getVertices(void) const {return mVertices;}
		const GLfloat* getTexCoords(void) const {return mTexCoords;}
		const GLfloat* getColors(void) const {return mColors;}
		const GLushort* getIndices(void) const {return mIndices;}

		GLuint getTextureID(void) const {return mTextureID;}

		void setTextureFiltering(TextureFiltering filtering);
		TextureFiltering getTextureFiltering(void) const;

		void getTextureImageDims(float dims[2]);

		void getWindowDims(float dims[2]);
	
	private:
		unsigned int oldSize;
	
		typedef std::list<Quad*> QuadList;
		QuadList mQuads;

		bool mLocked;

		float mWindowDims[2];

		float mImageDims[2];

		bool mHasTexture;

		bool mHasTextureAlpha;

		void reconstructBuffers(void);

		GLfloat* mVertices;
		GLushort* mIndices;
		GLfloat* mTexCoords;
		GLfloat* mColors;

		TextureFiltering mTextureFiltering;

		GLuint mTextureID;
	};

	class Quad
	{
	public:
		Quad(QuadSet* parent)
			: mParent(parent)
			, mRotation(0.0f)
		{
			LFA_Assert(mParent);
			setColor(Color::White);
		}

		// utility to set all corners to a particular color
		void setColor(const Color& color)		{mColorBL = mColorBR = mColorTR = mColorTL = color;}
		//color individual corners
		void setTLColor(const Color& color)		{mColorTL = color;}
		void setTRColor(const Color& color)		{mColorTR = color;}
		void setBLColor(const Color& color)		{mColorBL = color;}
		void setBRColor(const Color& color)		{mColorBR = color;}

		void setRotation(float rotationDegrees);
		void setRotationRadian(float rotationRadian);
		float getRotation(void) const;

		void setScreenCoords(FloatCoords coords);
		void setPixelScreenCoords(FloatCoords coords);

		void setTextureCoords(FloatCoords coords);
		void setPixelTextureCoords(FloatCoords coords);

		FloatCoords getPixelScreenCoords(void) const;  //this will not update until Drawn!
		FloatCoords getPixelTextureCoords(void) const;

		FloatCoords getScreenCoords(void) const;
		FloatCoords getTextureCoords(void) const;

	private:

		//(0,0) is upper left of screen (1,1) is bottom right
		FloatCoords mScreenCoords;

		//(0,0) is lower left of image (1,1) is top right
		FloatCoords mTexImageCoords;

		// pivot 
		// This is relative to center of the quad
		// The angle is specified in degrees and uses the usual convention
		// of +v for counter-clockwise and -ve for clockwise
		float mRotation;

		// color
		Color mColorBL;
		Color mColorBR;
		Color mColorTR;
		Color mColorTL;

		friend class QuadSet;
		QuadSet* const mParent;
	};

}


#endif