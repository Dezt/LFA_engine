#pragma once
#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include "BlendMode.h"
#include "TextureFiltering.h"


namespace LFA
{
	enum mirrorMode
	{
		MIRROR_NONE = 0,
		MIRROR_HORZ,
		MIRROR_VERT,
		MIRROR_ALL
	};

	class IntCoords;
	class Image;
	class QuadSet;
	class DrawUtils
	{
	public:
		// default blend mode just draws on top
		static void draw(QuadSet* quadset, BlendMode blendMode=BLEND_NONE);

		static void takeScreenshot( const IntCoords &screenCoords, LFA::Image &img);

		static mirrorMode mirrorState; //can be used to flip the screen vertically, horizontally, or both

		//TODO
		//static void startRTT(IntCoords &screenCoords); //when start Render To Texture is called, nothing will draw to the screen until endRTT() is called.
		//static void endRTT(Image &img);

	private:
		static void setBlending(BlendMode blendMode);
		static void setTextureFiltering(TextureFiltering filterMode);

		//screenshot vars
		//	static bool screenshotActive;
		//	static LFA::IntCoords coords;
		//	static GLuint screenshotFBO;
		//	static GLuint screenshotTexture;
	};

	//DrawUtils::screenshotActive = false;
}
#endif