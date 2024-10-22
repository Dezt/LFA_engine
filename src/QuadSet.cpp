#include "LFASTDHeaders.h"
#include "LFAAssert.h"

#include "QuadSet.h"
#include "Image.h"
#include "LFAMath.h"
#include "TextureFiltering.h"
//After all includes:
#ifdef LFA_INSTRUMENT
#define new new(1, __FILE__, __LINE__)
#endif
namespace LFA
{
	QuadSet::QuadSet(int windowWidth, int windowHeight)
		: mLocked(false)
		, mVertices(0)
		, mIndices(0)
		, mTexCoords(0)
		, mColors(0)
		, mTextureFiltering(FILTERING_LINEAR)
	{
		mWindowDims[0] = (float)windowWidth;
		mWindowDims[1] = (float)windowHeight;

		mImageDims[0] = 0;
		mImageDims[1] = 0;

		mHasTexture = false;
		mHasTextureAlpha = false;
	}

	QuadSet::~QuadSet()
	{
		if (hasTexture())
		{
			clearTextureImage();
		}
		
		while(mQuads.empty()==false)
		{
			Quad* quad = mQuads.front();mQuads.pop_front();
			delete quad;
		}

		//Fix mem leak:
		delete [] mVertices;
		delete [] mIndices;
		delete [] mTexCoords;
		delete [] mColors;
	}

	void QuadSet::windowResized(int windowWidth, int windowHeight)
	{
		mWindowDims[0] = (float)windowWidth;
		mWindowDims[1] = (float)windowHeight;
		beginEdit();
		endEdit();
	}

	void QuadSet::getTextureImageDims(float dims[2])
	{
		LFA_Assert(hasTexture());
		for(unsigned int i = 0; i < 2; ++i)
		{
			dims[i] =  mImageDims[i];
		}
	}
	void QuadSet::getWindowDims(float dims[2])
	{
		for(unsigned int i = 0; i < 2; ++i)
		{
			dims[i] =  mWindowDims[i];
		}
	}

	void QuadSet::beginEdit(void)
	{
		mLocked=true;
		oldSize = getNumQuads();
	}

	static inline void makeRotationMatrix(float result[3][3], const float* quadCenter, float theta)
	{
		float translationToCenter[3][3];
		Math::makeIdentity(translationToCenter);

		translationToCenter[0][2]=-quadCenter[0];
		translationToCenter[1][2]=-quadCenter[1];

		float rotation[3][3];
		Math::makeIdentity(rotation);
		rotation[0][0]=cos(theta); rotation[0][1]=-sin(theta);
		rotation[1][0]=sin(theta); rotation[1][1]=cos(theta);

		float translationBack[3][3];
		Math::makeIdentity(translationBack);
		translationBack[0][2]= quadCenter[0];
		translationBack[1][2]= quadCenter[1];

		float interim[3][3];
		Math::mult(interim, rotation, translationToCenter);
		Math::mult(result, translationBack, interim);

	}

	static inline void apply2Drotation(const float rotMat[3][3], float* coords)
	{
		float newCoords[3];
		float coordvec[3];
		coordvec[0]=coords[0];
		coordvec[1]=coords[1];
		coordvec[2]=1.0f;
		Math::mult(newCoords, rotMat, coordvec);

		for(unsigned int i=0; i < 2; ++i)
		{
			coords[i] = newCoords[i];
		}
	}

	static inline void fillPosAndMarchOffset(float* toVerts, int& toOffset, float* fromVerts)
	{
		for(unsigned int i = 0; i < 3; ++i)
		{
			toVerts[toOffset + i]=fromVerts[i];
		}
		toOffset+=3;
	}

	static inline void fillTexCoordsAndMarchOffset(float* toTexCoords, int& toOffset, float* fromTexCoords)
	{
		for(unsigned int i = 0; i < 2; ++i)
		{
			toTexCoords[toOffset + i]=fromTexCoords[i];
		}
		toOffset+=2;
	}
	static inline void fillColorsAndMarchOffset(float* toColors, int& toOffset, const Color& fromColor)
	{
		for(unsigned int i = 0; i < 4; ++i)
		{
			toColors[toOffset + i]=fromColor[i];
		}
		toOffset+=4;
	}

	void QuadSet::reconstructBuffers(void)
	{
		delete [] mVertices;
		delete [] mIndices;
		delete [] mTexCoords;
		delete [] mColors;

		int numTriangles = mQuads.size()* 2; // each quad is 2 tris
		mVertices = new GLfloat[
			numTriangles
			* 3 // tri has 3 vertices
			* 3 // each vert has 3 values
		];

		mIndices = new GLushort[
			numTriangles
			* 3 // each tri has 3 indices
		];

		mTexCoords = new GLfloat[ 
			numTriangles
			* 3 // tri has 3 verts
			* 2 // each vert has 2 values for s and t respectively
		];

		mColors = new GLfloat[ 
			numTriangles
			* 3 // each triangle has 3 colors
			* 4 // each color has 4 values
		];
	}

	void QuadSet::endEdit(void)
	{
		LFA_Assert(mLocked);

		if (oldSize != getNumQuads())
		{
			reconstructBuffers();
		}

		int currentPosOffset = 0;
		float scratchPos[3];
		scratchPos[2]=0.0f;

		int currentTexCoordOffset = 0;
		float scratchTexCoords[2];

		int currentColorOffset = 0;

		int indexOffset = 0;
		int vertCount = 0;

		float pixelCenter[2];

		for(QuadList::const_iterator it = mQuads.begin(); it != mQuads.end(); ++it)
		{
			Quad* quad = *it;

			float rotationInRadians = Math::toRadians(quad->mRotation);

			pixelCenter[0] = quad->mScreenCoords.x + quad->mScreenCoords.width;
			pixelCenter[1] = quad->mScreenCoords.y - quad->mScreenCoords.height;
			Math::screenToPixelCoords(pixelCenter, mWindowDims);
			

			float rotMat[3][3];
			makeRotationMatrix(rotMat, pixelCenter, rotationInRadians);

			// Fill up the vertices
			{
				// BL
				scratchPos[0] = quad->mScreenCoords.x;
				scratchPos[1] = quad->mScreenCoords.y-quad->mScreenCoords.height*2;
				Math::screenToPixelCoords(scratchPos, mWindowDims);
				apply2Drotation(rotMat, scratchPos);
				Math::pixelsToScreenCoords(scratchPos, mWindowDims);
				fillPosAndMarchOffset(mVertices, currentPosOffset, scratchPos);

				// BR
				scratchPos[0] = quad->mScreenCoords.x+quad->mScreenCoords.width*2;
				scratchPos[1] = quad->mScreenCoords.y-quad->mScreenCoords.height*2;
				Math::screenToPixelCoords(scratchPos, mWindowDims);
				apply2Drotation(rotMat, scratchPos);
				Math::pixelsToScreenCoords(scratchPos, mWindowDims);
				fillPosAndMarchOffset(mVertices, currentPosOffset, scratchPos);

				// TR
				scratchPos[0] = quad->mScreenCoords.x+quad->mScreenCoords.width*2;
				scratchPos[1] = quad->mScreenCoords.y;
				Math::screenToPixelCoords(scratchPos, mWindowDims);
				apply2Drotation(rotMat, scratchPos);
				Math::pixelsToScreenCoords(scratchPos, mWindowDims);
				fillPosAndMarchOffset(mVertices, currentPosOffset, scratchPos);

				// TL
				scratchPos[0] = quad->mScreenCoords.x;
				scratchPos[1] = quad->mScreenCoords.y;
				Math::screenToPixelCoords(scratchPos, mWindowDims);
				apply2Drotation(rotMat, scratchPos);
				Math::pixelsToScreenCoords(scratchPos, mWindowDims);
				fillPosAndMarchOffset(mVertices, currentPosOffset, scratchPos);
			}

			// Fill up the texture coordinates
			{
				// BL
				scratchTexCoords[0] = quad->mTexImageCoords.x;
				scratchTexCoords[1] = quad->mTexImageCoords.y;
				fillTexCoordsAndMarchOffset(mTexCoords, currentTexCoordOffset, scratchTexCoords);

				// BR
				scratchTexCoords[0] = quad->mTexImageCoords.x+quad->mTexImageCoords.width;
				scratchTexCoords[1] = quad->mTexImageCoords.y;
				fillTexCoordsAndMarchOffset(mTexCoords, currentTexCoordOffset, scratchTexCoords);

				// TR
				scratchTexCoords[0] = quad->mTexImageCoords.x+quad->mTexImageCoords.width;
				scratchTexCoords[1] = quad->mTexImageCoords.y+quad->mTexImageCoords.height;
				fillTexCoordsAndMarchOffset(mTexCoords, currentTexCoordOffset, scratchTexCoords);

				// TL
				scratchTexCoords[0] = quad->mTexImageCoords.x;
				scratchTexCoords[1] = quad->mTexImageCoords.y+quad->mTexImageCoords.height;
				fillTexCoordsAndMarchOffset(mTexCoords, currentTexCoordOffset, scratchTexCoords);
			}

			// Fill up the colors
			{
				// BL
				fillColorsAndMarchOffset(mColors, currentColorOffset, quad->mColorBL);

				// BR
				fillColorsAndMarchOffset(mColors, currentColorOffset, quad->mColorBR);

				// TR
				fillColorsAndMarchOffset(mColors, currentColorOffset, quad->mColorTR);

				// TL
				fillColorsAndMarchOffset(mColors, currentColorOffset, quad->mColorTL);
			}
		
			// Fill up the indices
			{
				mIndices[indexOffset + 0] = vertCount + 0;
				mIndices[indexOffset + 1] = vertCount + 1;
				mIndices[indexOffset + 2] = vertCount + 3;

				mIndices[indexOffset + 3] = vertCount + 1;
				mIndices[indexOffset + 4] = vertCount + 2;
				mIndices[indexOffset + 5] = vertCount + 3;

				vertCount += 4;
				indexOffset += 6;
			}
			
		}
		mLocked = false;

	}

	// appended at the end of the list*/
	Quad* QuadSet::createQuad(void)
	{
		LFA_Assert(mLocked);
		if (!mLocked)
			return 0;

		Quad* quad = new Quad(this);

		mQuads.push_back(quad);

		return quad;
	}

	Quad* QuadSet::getQuad(unsigned int i) const
	{
		LFA_Assert(mLocked);
		if (!mLocked)
			return 0;
		//VDH FIX: Can happen if img load failed: (original and optimized Image::LoadPNG_forcePowerOfTwo, LoadPNG)
		LFA_Assert(i+1 <= mQuads.size() && "Index out of bounds!");

		unsigned int index = 0;
		QuadList::const_iterator it;
		for(it = mQuads.begin(); it != mQuads.end(); ++it)
		{
			if (index==i)
			{
				break;
			}
			++index;
		}

		LFA_Assert(it != mQuads.end());
		
		return *it; //VDH FIX: can have bad pointer (crash) if overlapping loading of assets
	}

	bool QuadSet::removeQuad(unsigned int i)
	{
		LFA_Assert(mLocked);
		if (!mLocked)
			return false;

		return removeQuad(getQuad(i));
	}

	bool QuadSet::removeQuad( Quad* q )
	{
		LFA_Assert(mLocked);
		if (!mLocked)
			return 0;

		QuadList::const_iterator it;
		for(it = mQuads.begin(); it != mQuads.end(); ++it)
		{
			if (*it == q)
			{
				mQuads.remove(q);
				delete q;    
				q = 0;
				return true;
			}
		}	
		return false;
	}

	void QuadSet::removeAllQuads(void)
	{
		LFA_Assert(mLocked);
		QuadList::const_iterator it;
		for(it = mQuads.begin(); it != mQuads.end(); ++it)
		{
			delete *it;
		}
		mQuads.clear();
	}
	
	void QuadSet::setTextureImage(std::string filePath)
	{
		Image image;
		if (image.Load(filePath)==Image::IMG_OK)
		{
			setTextureImage(image);		
		}
		else
		{
			std::string err("Could not load image: ");
			err.append(filePath);
			err.append("\n");
			printf(err.c_str());
		}
	}

	void QuadSet::setTextureImage(const Image& image)
	{
		glEnable(GL_TEXTURE_2D);
		if (hasTexture())
		{
			clearTextureImage();
		}

		glGenTextures(1, &mTextureID);
		glBindTexture(GL_TEXTURE_2D, mTextureID);

		int format = 0;
		if (image.GetBitsPerPixel()/8==3)
		{
			format = GL_RGB;
			mHasTextureAlpha = false;
		}
		else if (image.GetBitsPerPixel()/8==4)
		{
			format = GL_RGBA;
			mHasTextureAlpha = true;
		}
		LFA_Assert(format>0);

		glTexImage2D(GL_TEXTURE_2D
			, 0
			, format
			, image.GetWidth(), image.GetHeight()
			, 0 // border
			, format
			, GL_UNSIGNED_BYTE
			, image.GetImg());

		mHasTexture = true;

		mImageDims[0] = (float)image.GetWidth();
		mImageDims[1] = (float)image.GetHeight();
	}

	void QuadSet::clearTextureImage()
	{
		LFA_Assert(mHasTexture);
		glDeleteTextures(1, &mTextureID);
		mHasTexture=false;
		mHasTextureAlpha = false;
	}

	void QuadSet::setTextureFiltering(TextureFiltering filtering)
	{
		mTextureFiltering = filtering;
	}
	TextureFiltering QuadSet::getTextureFiltering(void) const
	{
		return mTextureFiltering;
	}


	void Quad::setRotation(float rotationDegree)
	{
		mRotation = rotationDegree;
	}

	void Quad::setRotationRadian( float rotationRadian )
	{
		mRotation = LFA::Math::toDegrees(rotationRadian);
	}

	float Quad::getRotation(void) const
	{
		return mRotation;
	}

	void Quad::setScreenCoords(FloatCoords coords)
	{
		mScreenCoords = coords;
	}
	void Quad::setPixelScreenCoords(FloatCoords coords)
	{
		LFA_Assert(mParent);

		float windowDims[2];
		mParent->getWindowDims(windowDims);

		float scratchPos[3];
		scratchPos[0] = coords.x;
		scratchPos[1] = windowDims[1]-coords.y;
		scratchPos[2] = 0.0f;
		Math::pixelsToScreenCoords(scratchPos, windowDims);

		mScreenCoords.x = scratchPos[0];
		mScreenCoords.y = scratchPos[1];

		mScreenCoords.width = coords.width/windowDims[0];
		mScreenCoords.height = coords.height/windowDims[1];
	}

	void Quad::setTextureCoords(FloatCoords coords)
	{
		mTexImageCoords = coords;
	}

	void Quad::setPixelTextureCoords(FloatCoords coords)
	{
		LFA_Assert(mParent && mParent->hasTexture());
		float imageDims[2];
		mParent->getTextureImageDims(imageDims);

		mTexImageCoords = Math::toFloatCoords(coords, imageDims); //this converts pixels floatCoords to UV/screen floatCoords
	}

	FloatCoords Quad::getPixelScreenCoords(void) const
	{
		LFA_Assert(mParent);

		float windowDims[2];
		mParent->getWindowDims(windowDims);

		float scratchPos[2];
		scratchPos[0] = mScreenCoords.x;
		scratchPos[1] = mScreenCoords.y;

		Math::screenToPixelCoords(scratchPos, windowDims);

		FloatCoords coords;

		coords.x = scratchPos[0];
		coords.y = windowDims[1] - scratchPos[1];

		coords.width = mScreenCoords.width*windowDims[0];
		coords.height = mScreenCoords.height*windowDims[1];

		return coords;
	}

	FloatCoords Quad::getPixelTextureCoords(void) const
	{
		// PPP: TO DO
		LFA_Assert(false);
		return FloatCoords();
	}

	FloatCoords Quad::getScreenCoords(void) const
	{
		return mScreenCoords;
	}
	FloatCoords Quad::getTextureCoords(void) const
	{
		return mTexImageCoords;
	}

}