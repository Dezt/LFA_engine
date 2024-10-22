#include "Image.h"

#include "Config.h"  //this class needs to make sure we get the proper preprocessor defines
#include <fstream>
#include <iostream>
#include "LFAAssert.h"
#include "lfalog.h"
#include "lfatrace.h"


namespace LFA
{


	class PngDecoder : public LodePNG::Decoder
	{
		public:
		void decode(unsigned char** out, size_t* outsize, const unsigned char* in, size_t insize)
		{
			LodePNG_Decoder_decode(this, out, outsize, in, insize);
		}


	};

	//Put on stack
	//Will free the memory when the function returns
	class PopFree
	{
	public:
		unsigned char* p;
		PopFree(){ p = NULL;}
		~PopFree(){ free();}
		void free()
		{  if(NULL!=p) 
			delete []p; 
			p=NULL;
		}


	};

	 /*
	loadFile  -- load a binary file into ram. (Can be used anywhere -- add to .h if so.)
	in: filename
	out: returns new buffer, length
	Errors: File could not be opened  buffer == NULL, length == 0;
	        Buffer allocation failed  buffer == NULL,  length == -1;
		    Read error                buffer == NULL,  length == -2;
	*/
	static  unsigned char* loadFile(const char* filename, int* length)   
	{
		unsigned char* pOut = NULL;
		FILE *fp = NULL;
		long sz = 0;
		*length = 0;
		fp = fopen(filename,"rb");
		if(NULL == fp)
		{
			//Could not open file
			return pOut;

		}
		//How big:
		fseek(fp, 0L, SEEK_END);
		sz = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		if(sz>0)
			pOut = new unsigned char[sz];

		//Alloc failed:
		if(NULL == pOut)
		{
			fclose(fp);
			*length = -1;
			return pOut;
		}

		size_t nread = fread(pOut,sizeof(*pOut),sz,fp);
		//Read failed:
		if (nread != sz)
		{
			fclose(fp);
			delete [] pOut;
			pOut = NULL;
			*length = -2;
			return pOut;
		}

		fclose(fp);

		*length = nread; 
		return pOut;
	  
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Image::Image()
	{
		pImage=0;
		width = height = bpp =0;

		//default to transparent magenta.  These is only meaningful if  LFA_FORCE_POWER_OF_TWO is set to true;
		fillColorR = 255;
		fillColorG = 0;
		fillColorB = 255;
		fillColorA = 0;
	}

	Image::~Image()
	{
		if (pImage)
			delete [] pImage;
	}


	bool LFA::Image::getColor( int x, int y, LFA::intColor &clr )
	{
		if ( x < 0 || x >= width || y < 0 || y >= height)
		{
			LOGWRN ("Warning!  attempted color read out of range!\n");
			clr = LFA::intColor::Transparent;
			return false;
		}

		y = height-1 - y; //remember ogl loads textures upside down
		unsigned long i = (( y  * width) + x) * bpp/8;

		clr.r = (unsigned int)pImage[i];
		i++;
		clr.g =(unsigned int)pImage[i];
		i++;
		clr.b = (unsigned int)pImage[i];	

		if (bpp == 32)
		{
			i ++;		
			clr.a = (unsigned int)pImage[i];
		}
		return true;
	}

	bool Image::setColor( int x,  int y, LFA::intColor &clr )
	{
		if (x < 0 || y < 0 ||  x >= width || y >= height)
		{
			LOGWRN ("Warning!  attempted color write out of range!\n");
			return false;
		}

		y = height-1 - y; //remember ogl loads textures upside down
		unsigned long i = (( y  * width) + x) * bpp/8;

		pImage[i] = clr.r;
		i++;
		pImage[i] = clr.g ;
		i++;
		pImage[i] = clr.b ;
		if (bpp == 32)
		{
			i ++;	
			pImage[i] = clr.a;
		}
		return true;
	}


	int Image::Load(const std::string &filePath)
	{	
		int response = Load(filePath.c_str(), IMG_TYPE_PNG); //try as a png first.
		if ( response == IMG_OK)  
			return IMG_OK;
		else if (response == IMG_ERR_NO_FILE)// don't try more formats if the file doesn't exist at all.
			return IMG_ERR_NO_FILE;
		else if (response == IMG_ERR_UNSUPPORTED)// don't try more formats if we found it but it was unloadable.
			return IMG_ERR_UNSUPPORTED;
		else if (Load(filePath.c_str(), IMG_TYPE_TGA) == IMG_OK)
			return IMG_OK;

		return IMG_LOAD_FAILED;
	}


	int Image::Load( const std::string &filePath, imageTypes asType )
	{
		return Load(filePath.c_str(), asType);
	}



int Image::Load(const char* filePath, imageTypes asType)
{
//VDH Removed not needed "check if file exists" 

#if LFA_FORCE_POWER_OF_TWO == 1 
	if (asType == IMG_TYPE_PNG)
		return LoadPNG_forcePowerOfTwo(filePath);
	else if (asType == IMG_TYPE_TGA)
		return LoadTGA_forcePowerOfTwo(filePath);
	else
		LFA_Assert(false);
#else
	if (asType == IMG_TYPE_PNG)
		return LoadPNG(filePath);
	else if (asType == IMG_TYPE_TGA)
		return LoadTGA(filePath);
	else
		LFA_Assert(false);
#endif

	return IMG_LOAD_FAILED;
}

bool Image::Load( unsigned int _width, unsigned int _height, unsigned int _bpp, unsigned char * pixels )
{
	if ((_bpp/8 != 3)&&(_bpp/8 !=4))
	{
		LOGERR("Unsupported color format: "<< _bpp << " bpp.");
		return false;
	}

	if(pImage)
	{
		delete [] pImage;
		pImage = 0;
	}

	width = (int)_width;
	height = (int)_height;
	bpp = _bpp;

	unsigned int size = width * height * (bpp/8);

	pImage=new unsigned char[size];
	memcpy(pImage, pixels, size);

	return true;
}


int Image::LoadTGA( const char* filePath )
{
	imageTGA img;
	if (img.Load(filePath)==imageTGA::IMG_OK)
	{
		if(pImage)
		{
			delete [] pImage;
			pImage = 0;
		}

		width = img.GetWidth();
		height = img.GetHeight();
		bpp = img.GetBitsPerPixel();

		pImage=new unsigned char[img.getImgSize()];
		memcpy(pImage, img.GetImg(), img.getImgSize());

		return IMG_OK;
	}
	else 
		return IMG_LOAD_FAILED;
}




 


/*
Load PNG image
in:		filePath -filename
out: 
		this->width
		this->height
		this->bpp
		this->pImage //array of the decoded image
*/
	int Image::LoadPNG( const char* filePath)
	{
		//load and decode
		PngDecoder pngDecoder;

		//PNG decoded, stored in:
		PopFree img;
		//unsigned char* pImgArr = NULL;
		size_t outsize=0;

		//Temporary
		PopFree buf;
		int bufLen = 0;

		//NOTE "SLOW" section of code somewhere from here to
		//"FAST"
		//Probably: a bit in file load (nothing can be done), most in decode
		//
		buf.p = loadFile(filePath,&bufLen);
		

		

		if(NULL!= buf.p && bufLen>0)
		{
			
			pngDecoder.decode(&img.p, &outsize, buf.p, bufLen);
			//Done with temporary buffer:
			buf.free();
		}
		else
		{
			
			LOGERR("Could not read file: error ("<<bufLen<<") "<< filePath);
			
			return IMG_LOAD_FAILED;
		}



		//if there's an error, display it, otherwise display information about the image
		if(pngDecoder.hasError()) 
		{
			LOGERR("Could not load PNG: "<< filePath <<" decoder ERROR: " << pngDecoder.getError() );
			
			return IMG_LOAD_FAILED;
		}
		else
		{
			//PNG types
			//					indexed		normal		withAlpha		note: indexed with alpha is same as indexed.
			//bpp					8				24				32					
			//depth				8				8				8
			//channels			1				3				4
			//colorType		3				2				6

			//get width, height and pixels
			width = pngDecoder.getWidth(), 
			height =  pngDecoder.getHeight();

			bpp = pngDecoder.getBpp();
			//we are not supporting gray scale or true indexing... uncompress to rgb in memory.
			if (bpp == 8 )
			{
				//check to see if we have a transparent color in this indexed png.
				unsigned int paletteSize = pngDecoder.getInfoPng().color.palettesize;
				unsigned char* pPalette = pngDecoder.getInfoPng().color.palette;
				for (unsigned int i = 0; i < paletteSize; i ++)
				{
					if ((int)pPalette[(i*4) + 3] == 0)
					{
						bpp = 32;  //a transparent, indexed png
						break;
					}		
				}
			}
			if (bpp == 8 )
				bpp = 24;  //an indexed png, with no transparency.	


			if ((bpp/8 != 3)&&(bpp/8 !=4))
			{
				LOGERR("Unsupported PNG color format: "<< filePath);
				
				return IMG_ERR_UNSUPPORTED;  //only allow rgb or rgba
			}
			
			if(pImage)
			{
				delete [] pImage;
				pImage = NULL;
			}
			pImage = new unsigned char[bpp/8 * width * height];

			//load the png into the 'image' memory
			//"FAST" has been optimized:
			unsigned long long  i = 0;
			int index = 0;			
			unsigned char* pImgArr = img.p;

			for( long int y = height -1; y  >= 0; y --)
			{

				for(long int x = 0; x  < width; x ++)
				{
					//get RGBA components
					index = 4 * y * width + 4 * x;
					pImage[i++] = pImgArr[index]; //red
					pImage[i++] = pImgArr[++index]; //green
					pImage[i++] = pImgArr[++index]; //blue	
					if (bpp == 32)
					{
						pImage[i++] = pImgArr[++index]; //alpha
					}
				}


			}

		}
		
		return IMG_OK;
	}




	int Image::LoadTGA_forcePowerOfTwo( const char* filePath )
	{


		//TODO


		return LoadTGA(filePath );
	}

/*
Load PNG image --expand it so width & height are a multiple of 2
in:		filePath -filename
out: 
		this->width
		this->height
		this->bpp
		this->pImage //array of the decoded image
*/
int Image::LoadPNG_forcePowerOfTwo( const char* filePath )
{

		//load and decode
		PngDecoder pngDecoder;

		//PNG decoded, stored in:
		PopFree img;
		//unsigned char* pImgArr = NULL;
		size_t outsize=0;

		//Temporary
		PopFree buf;
		int bufLen = 0;

		//NOTE "SLOW" section of code somewhere from here to
		//"FAST"
		//Probably: a bit in file load (nothing can be done), most in decode
		//
		buf.p = loadFile(filePath,&bufLen);
		

		

		if(NULL!= buf.p && bufLen>0)
		{
			
			pngDecoder.decode(&img.p, &outsize, buf.p, bufLen);
			//Done with temporary buffer:
			buf.free();
		}
		else
		{
			LOGERR("Could not read file: error ("<<bufLen<<") "<< filePath);
			
			return IMG_LOAD_FAILED;
		}


	//if there's an error, display it, otherwise display information about the image
	if(pngDecoder.hasError()) 
	{

		LOGERR("Could not load PNG: "<< filePath <<" decoder ERROR: " << pngDecoder.getError() );

		if ((int)pngDecoder.getError() == 29)
		{
			LOGERR("Your PNG has failed to load with error 29.");
			LOGERR("Error 29 signifies a bad header chunk, likely caused by XCODE compressing the PNG.");
			LOGERR("This XCODE optimization can be disabled by adding the following user variable to the target:");
			LOGERR("key: IPHONE_OPTIMIZE_OPTIONS    value: -skip-PNGs\n");
		}
		return IMG_LOAD_FAILED;
	}
	else
	{
		//PNG types
		//					indexed		normal		withAlpha		note: indexed with alpha is same as indexed.
		//bpp					8				24				32					
		//depth				8				8				8
		//channels			1				3				4
		//colorType		3				2				6

		bpp = pngDecoder.getBpp();
		//we are not supporting gray scale or true indexing... uncompress to rgb in memory.
		if (bpp == 8)
		{
			//check to see if we have a transparent color in this indexed png.
			unsigned int paletteSize = pngDecoder.getInfoPng().color.palettesize;
			unsigned char* pPalette = pngDecoder.getInfoPng().color.palette;
			for (unsigned int i = 0; i < paletteSize; i ++)
			{
				if ((int)pPalette[(i*4) + 3] == 0)
				{
					bpp = 32;  //a transparent, indexed png
					break;
				}		
			}
		}
		if (bpp == 8 )
			bpp = 24;  //an indexed png, with no transparency.

		if (bpp != 24 && bpp != 32)
		{

			LOGERR("Unsupported PNG color format: "<< filePath );
			return IMG_ERR_UNSUPPORTED;  //only allow rgb or rgba
		}

		//in order to force this image to be power of 2 we will have to see how much to extend it if necessary
		width = pngDecoder.getWidth();
		int targetWidth = 2;
		for (; width != targetWidth; targetWidth*=2) 
		{
			if (width < targetWidth)
				break;
		}
		height = pngDecoder.getHeight();
		int targetHeight = 2;
		for (; height != targetHeight; targetHeight*=2) 
		{
			if (height < targetHeight)
				break;
		}

		if(pImage)
		{
			delete [] pImage;
			pImage = 0;
		}
		pImage = new unsigned char[bpp/8 * targetWidth * targetHeight];

		int yOffset = targetHeight - height;  //this is to adjust the position of the image so that it will appear on the lower left of the actual texture
		//load the png into the 'image' memory
		unsigned long long  i = 0;
		int index = 0;			
		unsigned char* pImgArr = img.p;
		int yoffwid = 0;
		for( long int y = targetHeight -1; y  >= 0; y --)
		{
			yoffwid = (y - yOffset)* width;
			for(long int x = 0; x  < targetWidth; x ++)
			{
				index = 4 * (yoffwid + x);
				if ( x < width  && y > yOffset - 1)  //draw the real image in this section of the screen
				{
					//get RGBA components
					pImage[i++] = pImgArr[index]; //red
					pImage[i++] = pImgArr[++index]; //green
					pImage[i++] = pImgArr[++index]; //blue	
					if (bpp == 32)
					{
						pImage[i++] = pImgArr[++index]; //alpha
					}

				}
				else  //fill with a color
				{
					pImage[i] = fillColorR; i++;
					pImage[i] = fillColorG; i++;
					pImage[i] = fillColorB; i ++;		
					if (bpp == 32)
					{pImage[i] = fillColorA; i ++;}	
				}
			}
		}
		width = targetWidth;
		height = targetHeight;

	} //else good png load
	return IMG_OK;
}



void Image::mirrorImageX( void )
{
	if (!pImage)
	{
		LOGWRN("[LFA::Image] No image to mirrorX!");
		return;
	}
	unsigned int depth = bpp/8;
	unsigned long int imageSize = depth * width * height;

	//images are stored in memory 'upside down'  we have to flip it if we are going to save this data out to disk.
	unsigned char *flippedImagePixels = new unsigned char[imageSize];
	unsigned long int src = 0;
	unsigned long int dst = 0;
	
	for ( int y = 0; y < height; y++)
	{
		for ( int x = 0; x < width; x++)
		{
			//src = (( y  * width) + x) * depth; //normal copy
			src = (( y  * width) + width - x -1) * depth;

			flippedImagePixels[dst] = pImage[src];
			dst ++; src ++;
			flippedImagePixels[dst] = pImage[src];
			dst ++; src ++;
			flippedImagePixels[dst] = pImage[src];
			dst ++; src ++;
			if (bpp == 32)
			{
				flippedImagePixels[dst] = pImage[src];
				dst ++; 
			}
		}
	}

	memcpy(pImage, flippedImagePixels, imageSize);

	delete [] flippedImagePixels;
	flippedImagePixels = 0;
}


void Image::mirrorImageY( void )
{
	if (!pImage)
	{
		LOGWRN("[LFA::Image] No image to mirrorX!");
		return;
	}
	unsigned int depth = bpp/8;
	unsigned long int imageSize = depth * width * height;

	//images are stored in memory 'upside down'  we have to flip it if we are going to save this data out to disk.
	unsigned char *flippedImagePixels = new unsigned char[imageSize];
	unsigned long int src = 0;
	unsigned long int dst = 0;

	for ( int y = 0; y < height; y++)
	{
		for ( int x = 0; x < width; x++)
		{
			//src = (( y  * width) + x) * depth; //normal copy
			src = (( (height - y - 1)  * width) + x) * depth;

			flippedImagePixels[dst] = pImage[src];
			dst ++; src ++;
			flippedImagePixels[dst] = pImage[src];
			dst ++; src ++;
			flippedImagePixels[dst] = pImage[src];
			dst ++; src ++;
			if (bpp == 32)
			{
				flippedImagePixels[dst] = pImage[src];
				dst ++; 
			}
		}
	}

	memcpy(pImage, flippedImagePixels, imageSize);

	delete [] flippedImagePixels;
	flippedImagePixels = 0;
}




bool Image::saveToFilePNG( const std::string &filePath )
{
	if (!pImage)
	{
		LOGERR("[LFA::Image] No image to save!");
		return false;
	}

	//color Type
	//  0: greyscale, bit depths 1, 2, 4, 8, 16
	//	2: RGB, bit depths 8 and 16
	//	3: palette, bit depths 1, 2, 4 and 8
	//	4: greyscale with alpha, bit depths 8 and 16
	//	6: RGBA, bit depths 8 and 16
	unsigned int colorType = 2;
	if (bpp == 32)
		colorType = 6;
	
	unsigned int error = LodePNG_encode_file(filePath.c_str(), pImage, width, height, colorType, 8); 
	if (error) 
	{
		LOGERR("LodePNG file save failed with Error Code: "<< error);
		return false;
	}
	return true;
}




void Image::setFillColor( unsigned int r, unsigned int g, unsigned int b, unsigned int a /*= 0*/ )
{
	fillColorR = r;
	fillColorG = g;
	fillColorB = b;
	fillColorA = a;
}

Image & Image::operator=( const Image &img )
{
	width = img.GetWidth();
	height = img.GetHeight();
	bpp = img.GetBitsPerPixel();

	if(pImage)
	{
		delete [] pImage;
		pImage = 0;
	}
	unsigned long int imageSize = bpp/8 * width * height;
	pImage=new unsigned char[imageSize];

	memcpy(pImage, img.GetImg(), imageSize);

	return *this;
}




} //namespace LFA
