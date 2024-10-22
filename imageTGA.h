#pragma once
#ifndef _TGA_LOADER_H_
#define _TGA_LOADER_H_

// Modified code from: http://gpwiki.org/index.php/OpenGL:Tutorials:Tutorial_Framework:Ortho_and_Alpha
// by Karl Walsh
// TGA Loader - 16/11/04 Codehead

#include <string> //added this for convenience

namespace LFA
{
	class imageTGA
	{
	public:
		imageTGA();
		~imageTGA();
		int Load(std::string fileName);
		int Load(const char* szFilename);
		int GetBitsPerPixel() const;
		int GetWidth() const;
		int GetHeight() const;
		unsigned char* GetImg() const;       // Return a pointer to image data
		unsigned char* GetPalette();   // Return a pointer to VGA palette
		unsigned long getImgSize();
		enum 
		{ 
			IMG_OK = 1
			,IMG_ERR_NO_FILE
			,IMG_ERR_MEM_FAIL
			,IMG_ERR_BAD_FORMAT
			,IMG_ERR_UNSUPPORTED
		};

	private:
		short int iWidth,iHeight,iBPP;
		unsigned long lImageSize;
		char bEnc;
		unsigned char *pImage, *pPalette, *pData;

		// Internal workers
		int ReadHeader();
		int LoadRawData();
		int LoadTgaRLEData();
		int LoadTgaPalette();
		void BGRtoRGB();
		void FlipImg();
	};

}


#endif //_TGA_LOADER_H_