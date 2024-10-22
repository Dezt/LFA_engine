#pragma once
#ifndef IMAGE_H
#define IMAGE_H

// Modified code from: http://gpwiki.org/index.php/OpenGL:Tutorials:Tutorial_Framework:Ortho_and_Alpha
// by Karl Walsh
// TGA Loader - 16/11/04 Codehead

#include "LFASTDHeaders.h"
#include "imageTGA.h"
#include "lodepng.h"
#include "intColor.h"

namespace LFA
{

	enum imageTypes
	{
		IMG_TYPE_TGA = 1,
		IMG_TYPE_PNG,

		//The following are not available in LFA Engine by default.  To use them, you must extend image.
		IMG_TYPE_GIF = 100   
	};

	class Image
	{
	public:
		Image();
		~Image();
		virtual int Load(const std::string &filePath); //this override will try .png first,  then .tga
		virtual int Load(const std::string &filePath, imageTypes asType); 
		virtual int Load(const char* filePath, imageTypes asType);
		virtual bool Load(unsigned int _width, unsigned int _height, unsigned int _bpp, unsigned char * pixels); //raw loading,  returns false if failed

		int GetWidth()  const  {return width;}
		int GetHeight()	const {return height;}
		int GetBitsPerPixel() const {return bpp;}
		unsigned char* GetImg() const  {return pImage;}     // Return a pointer to image data

		bool setColor( int x, int y, LFA::intColor &clr); //returns false if xy coords were outside image.
		bool getColor( int x, int y, LFA::intColor &clr ); //returns false if xy coords were outside image.

		void mirrorImageX(void);
		void mirrorImageY(void);

		bool saveToFilePNG(const std::string &filePath); //will save this texture out to a PNG

		enum 
		{ 
			IMG_OK = 1
			,IMG_ERR_NO_FILE
			,IMG_LOAD_FAILED
			,IMG_ERR_UNSUPPORTED
		};

		Image & operator=(const Image &img);

		void setFillColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 0);

	protected:
		int LoadTGA(const char* filePath);
		int LoadPNG(const char* filePath);

		 //these are useful on the iphone,  since non power of 2 textures wont show up at all.   //to use, #define LFA_FORCE_POWER_OF_TWO 
		int LoadTGA_forcePowerOfTwo(const char* filePath); 
		int LoadPNG_forcePowerOfTwo(const char* filePath);
		unsigned int fillColorR;
		unsigned int fillColorG;
		unsigned int fillColorB;
		unsigned int fillColorA;

		int width, height, bpp;
		unsigned char *pImage;

		

	
	};

}


#endif //_TGA_LOADER_H_