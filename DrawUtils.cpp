#include "Config.h"
#include "LFASTDHeaders.h"
#include "LFAAssert.h"
#include "DrawUtils.h"

#include "QuadSet.h"
#include "IntCoords.h"
#include "Image.h"


namespace LFA
{
	mirrorMode DrawUtils::mirrorState = MIRROR_NONE; //default to no mirroring 


	void DrawUtils::setBlending(BlendMode blendMode)
	{
		switch (blendMode)
		{
		case BLEND_NONE:
			glDisable(GL_BLEND);
			break;
		case BLEND_ALPHA:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BLEND_ADD:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		case BLEND_MULT:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
		default:
			LFA_Assert(false);
		}
	}

	void DrawUtils::setTextureFiltering(TextureFiltering filterMode)
	{
		int filtering = 0;
		switch(filterMode)
		{
		case FILTERING_NONE:
			filtering = GL_NEAREST;
			break;
		case FILTERING_LINEAR:
			filtering = GL_LINEAR;
			break;
		default:
			LFA_Assert(false);
			break;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
	}
	void DrawUtils::draw(QuadSet* quadset, BlendMode blendMode)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, 0.0);

		if (mirrorState == MIRROR_HORZ)
			glScalef(-1.0f, 1.0f, 1.0f); 
		else if (mirrorState == MIRROR_VERT)
			glScalef(1.0f, -1.0f, 1.0f);
		else if (mirrorState == MIRROR_ALL)
			glScalef(-1.0f, -1.0f, 1.0f); 

		// always draw on top, else you get z-fighting
		glDisable(GL_DEPTH_TEST);

		setBlending(blendMode);
		
		if (quadset->hasTexture())
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, quadset->getTextureID());
			setTextureFiltering(quadset->getTextureFiltering());
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}

		// Enable VERTEX array
		glEnableClientState(GL_VERTEX_ARRAY);

		// Set pointers to vertices and texcoords
		glVertexPointer(3, GL_FLOAT, 0, quadset->getVertices());
		// Enable vertices and texcoords arrays
		glEnableClientState(GL_VERTEX_ARRAY);

		if (quadset->hasTexture())
		{
			glTexCoordPointer(2, GL_FLOAT, 0, quadset->getTexCoords());
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
		}

		glColorPointer(4, GL_FLOAT, 0, quadset->getColors());
		glEnableClientState(GL_COLOR_ARRAY);

		glDrawElements(GL_TRIANGLES
			, quadset->getNumQuads()*6 // each quad has 4 indices
			, GL_UNSIGNED_SHORT
			, quadset->getIndices());

		glDisableClientState(GL_VERTEX_ARRAY);
		if (quadset->hasTexture())
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glDisableClientState(GL_COLOR_ARRAY);

		// re-enable the old depth fighting
		// Enables Depth Testing
		glEnable(GL_DEPTH_TEST);
		// The Type Of Depth Test To Do
		glDepthFunc(GL_LEQUAL);

	}


	void DrawUtils::takeScreenshot( const IntCoords &screenCoords, Image &img )
	{
		unsigned char *pixels = new unsigned char[screenCoords.width*screenCoords.height*4];
		glReadPixels(screenCoords.x, screenCoords.y, screenCoords.width, screenCoords.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		img.Load(screenCoords.width, screenCoords.height, 32, pixels);

		delete [] pixels;
		pixels = 0;
	}


/*	void DrawUtils::startRTT( IntCoords &screenCoords )
	{

		if (screenshotActive)
		{
			LOGWRN("Screen shot rendering already started!");
			return;
		}

		screenshotActive = true;
		glGenTextures( 1, &screenshotTexture );
		glGenFramebuffersEXT(1, &screenshotFBO);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, screenshotTexture, 0);

	}

	void DrawUtils::endRTT( Image &img )
	{
		if (!screenshotActive)
		{
			LOGWRN("Screen shot rendering had already ended or was not active!");
			return;
		}

		GLubyte *pixels = new GLubyte[w*h*4];
		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); 
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

		glDeleteTextures(1, screenshotTexture);
		glDeleteFramebuffersEXT(1, screenshotFBO);
		screenshotActive = false;
	}*/
}
