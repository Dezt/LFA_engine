//S.U.C.C.  2D
//Simple Unfeatured Collision Class

//created by metaldev.  2009
//the license for using this software means you must  keep this paragraph intact on any copies or modifications of this class.
//Otherwise, there are no limitations on the use of this free open source software,  use for anything you like!  Commercial or not.
//for questions or support use the forums at www.lfagames.com 

#pragma once
#ifndef LFA_COLLISION_MANAGER_H
#define LFA_COLLISION_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>  
#include <vector>
#include <math.h>

#include "LFAMath.h"  //uses floatToInt()


// Forward declarations
//class TiXmlDocument;
//class TiXmlElement;

namespace LFA
{
	enum collisionShapeType
	{
		COL_ERROR = 0,
		COL_BOX,
		COL_CIRCLE
	};
	
	class collisionShape
		{
		public:
			collisionShape(){}
			~collisionShape(){}
			virtual collisionShapeType getType(void) {return COL_ERROR;}
			
			std::string name;	    //name of the collision shape
			std::string var;		//var of the collision shape					
		};
	
	class collisionBox : public collisionShape
	{
	public:
		collisionBox(){xmin, ymin, xmax, ymax = 0;}
		~collisionBox(){}
		collisionShapeType getType(void) {return COL_BOX;}
		float xmin;
		float ymin;
		float xmax;
		float ymax;
	};	
	
	class collisionCircle : public collisionShape
		{
		public:
			collisionCircle(){posX, posY, radius = 0;}
			~collisionCircle(){}			
			collisionShapeType getType(void) {return COL_CIRCLE;}
			float posX;
			float posY;
			float radius;
		};	
		
	class collisionHit
	{
		friend class cCollisionManager;
	public:
		collisionHit(){posX, posY, distance, angle = 0;}
		~collisionHit(){}
		
		std::string name;			//name of the collision cube we collided with
		std::string var;			//var of the collision cube we collided with
		float posX;
		float posY;
		float angle;	//if the hit was generated from a line collision,  this will contain a normal facing a way from the surface.

	private:
		float distance;	//distance from the first given point, represented as t.  this is NOT unit distance,  its used for sorting the hits in the array.
	};	
	
	
	
	
	class cCollisionManager
		{
		public:
			cCollisionManager();
			~cCollisionManager();
			
			//void loadCollisionFile( const Ogre::String &collisionFileName, const Ogre::String &groupName);
			
			//COLLISION DETECTION against loaded xml collisions
			std::vector<collisionHit> pointCollide(float x, float y);	
			
			std::vector<collisionBox> boxCollide(collisionBox &cBox);
			std::vector<collisionBox> boxCollide(float xmax, float ymax, float xmin, float ymin);		
			
			std::vector<collisionHit> lineCollide(float x1, float y1, float x2, float y2);	
			
			
			//shape vs shape collisions
			bool pointVsBox (float x, float y, collisionBox &cBox);
			bool pointVsBox (float x, float y, float xmax, float ymax, float xmin, float ymin);	
			
			bool pointVsCircle(float x, int&y, collisionCircle &cCircle);
			bool pointVsCircle(float x, int&y, float circleX, float circleY, float radius);
			
			bool boxVsBox (collisionBox &cBox1, collisionBox &cBox2);
			bool boxVsBox (collisionBox &cBox, float xmax, float ymax, float xmin, float ymin);
			bool boxVsBox (float xmax1, float ymax1, float xmin1, float ymin1, float xmax2, float ymax2, float xmin2, float ymin2);
			
			bool boxVsCircle(collisionBox &cBox, collisionCircle &cCircle);
			bool boxVsCircle(float xmax, float ymax, float xmin, float ymin, float circleX, float circleY, float radius);
			
			bool circleVsCircle(collisionCircle &cCircle1, collisionCircle &cCircle2);
			bool circleVsCircle(collisionCircle &cCircle1, float circleX, float circleY, float radius);
			bool circleVsCircle(float circle1X, float circle1Y, float radius1, float circle2X, float circle2Y, float radius2);			
						
			void rayVsBox (float x1, float y1,  float x2, float y2, collisionBox &cBox, std::vector<collisionHit> &hits); 
			void rayVsBox(float x1, float y1,  float x2, float y2,  float xmax, float ymax, float xmin, float ymin, std::vector<collisionHit> &hits);
			
			//todo
			std::vector<collisionHit> rayVsCircle (float x1, float y1,  float x2, float y2, collisionCircle &cCircle);
			std::vector<collisionHit> rayVsCircle (float x1, float y1,  float x2, float y2, float circleX, float circleY, float radius);
			

			
			
			void orderCollisionHitsByDistance(std::vector<collisionHit> *hits); 
			
			//convert 2 points into a collisionBox
			collisionBox rayToBox(float x1, float y1, float x2, float y2);
			
			float getDistance(float x1, float y1, float x2, float y2);
			
			
		private:
			std::vector<collisionBox> colBoxes;	
			bool oneDimentionalTest( float min1, float max1,  float min2, float max2);		
			
			//std::string getAttrib(TiXmlElement *XMLNode, const std::string &parameter, const std::string &defaultValue = "");
			//Ogre::Real getAttribReal(TiXmlElement *XMLNode, const std::string &parameter, Ogre::Real defaultValue = 0);
		};
	
}


#endif //LFA_COLLISION_MANAGER_H