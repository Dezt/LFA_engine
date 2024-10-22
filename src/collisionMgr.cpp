//#include "tinyxml.h"
#include "collisionMgr.h"


using namespace LFA;
using namespace std;


cCollisionManager::cCollisionManager()
{
}

cCollisionManager::~cCollisionManager()
{
}

/*
void cCollisionManager::loadCollisionFile( const Ogre::String &collisionFileName, const Ogre::String &groupName )
{
	//clear the mCollision vector
	mCollision->clear();
	
	TiXmlDocument   *XMLDoc = 0;
	TiXmlElement   *XMLRoot;
	
	try
	{
		DataStreamPtr pStream = ResourceGroupManager::getSingleton().
		openResource( collisionFileName, groupName );
		
		String data = pStream->getAsString();
		// Open the .col File
		XMLDoc = new TiXmlDocument();
		XMLDoc->Parse( data.c_str() );
		pStream->close();
		pStream.setNull();
		
		if( XMLDoc->Error() )
		{
			//We'll just log, and continue on gracefully
			LogManager::getSingleton().logMessage("[collisionLoader] The TiXmlDocument reported an error");
			delete XMLDoc;
			XMLDoc = 0;
			return;
		}
	}
	catch(...)
	{
		//We'll just log, and continue on gracefully
		LogManager::getSingleton().logMessage("[collisionLoader] Error creating TiXmlDocument");
		delete XMLDoc;
		XMLDoc = 0;
		return;
	}
	
	// Validate the File
	XMLRoot = XMLDoc->RootElement();
	if( String( XMLRoot->Value()) != "col"  ) {
		LogManager::getSingleton().logMessage( "[collisionLoader] Error: Invalid .col File. Missing <col>" );
		delete XMLDoc;
		XMLDoc  = 0;  
		XMLRoot = 0;
		return;
	}
	
	// parse here into mCollision struct
	TiXmlElement *pElement = XMLRoot->FirstChildElement("branch");
	if(pElement)
	{
		while(pElement)
		{	
			collisionBranch cBranch;
			
			cBranch.name = getAttrib(pElement, "name");
			cBranch.xmin = getAttribReal( pElement, "xmin") ;
			cBranch.ymin = getAttribReal( pElement, "ymin") ;
			cBranch.zmin = getAttribReal( pElement, "zmin") ;
			cBranch.xmax = getAttribReal( pElement, "xmax") ;
			cBranch.ymax = getAttribReal( pElement, "ymax") ;
			cBranch.zmax = getAttribReal( pElement, "zmax") ;
			
			TiXmlElement *pChildElement = pElement->FirstChildElement("box");
			while(pChildElement)
			{
				collisionBox cBox;
				
				cBox.name = getAttrib(pChildElement, "name");
				cBox.var = getAttrib(pChildElement, "var");
				cBox.xmin = getAttribReal( pChildElement, "xmin") ;
				cBox.ymin = getAttribReal( pChildElement, "ymin") ;
				cBox.zmin = getAttribReal( pChildElement, "zmin") ;
				cBox.xmax = getAttribReal( pChildElement, "xmax") ;
				cBox.ymax = getAttribReal( pChildElement, "ymax") ;
				cBox.zmax = getAttribReal( pChildElement, "zmax") ;
				
				cBranch.colBoxes.push_back(cBox);
				pChildElement = pChildElement->NextSiblingElement("box");
			}		
			
			mCollision->push_back(cBranch);
			pElement = pElement->NextSiblingElement("branch");
		}
	}
	
	LogManager::getSingleton().logMessage( "[collisionLoader] Loaded collision successfully" );
	// Close the XML File
	delete XMLDoc;
	XMLDoc  = 0; 
}

*/
std::vector<collisionHit> cCollisionManager::pointCollide(float x, float y)
{
	std::vector<collisionHit> vCollisions;
	

	vector<collisionBox>::iterator box_iter = colBoxes.begin();
		
	for(; box_iter != colBoxes.end(); ++box_iter)
	{
		if (pointVsBox(x, y, (*box_iter)))
		{
			collisionHit hit;			// add this box as a hit if there is a collision
			
			hit.name = (*box_iter).name;
			hit.var = (*box_iter).var;
			hit.posX = x;
			hit.posY = y;
			hit.angle = 0;
			hit.distance = 0;
						
			vCollisions.push_back(hit);
		}
	}
	return vCollisions;
}

std::vector<collisionBox> cCollisionManager::boxCollide(collisionBox &cBox)
{
	return boxCollide(cBox.xmax, cBox.ymax, cBox.xmin, cBox.ymin);
}

std::vector<collisionBox> cCollisionManager::boxCollide(float xmax, float ymax, float xmin, float ymin)
{
	std::vector <collisionBox> vCollisions;
	


				vector<collisionBox>::iterator box_iter = colBoxes.begin();
				
				for(; box_iter != colBoxes.end(); ++box_iter)
				{					
					//collide box with boxes
					if ( boxVsBox((*box_iter),xmax, ymax, xmin, ymin ))
					{
						vCollisions.push_back(*box_iter);
					}
				}
			
		
	
	return vCollisions;
}



std::vector<collisionHit> cCollisionManager::lineCollide(float x1, float y1, float x2, float y2)
{
	std::vector <collisionHit> vCollisions;
	std::vector <collisionHit> hits; //temp variable
	
	if (colBoxes.size())
	{
		
		collisionBox rayBBox =  rayToBox(x1,y1,x2,y2);
		
				vector<collisionBox>::iterator box_iter = colBoxes.begin();
				vector<collisionBox>::iterator box_end = colBoxes.end();
				
				for(; box_iter != box_end; ++box_iter)
				{
					//collide with each box					
					if (boxVsBox((*box_iter), rayBBox))
					{
						hits.clear();
						rayVsBox(x1,y1,x2,y2,(*box_iter), hits);
						
						if (hits.size())
						{
							vector<collisionHit>::iterator hits_iter = hits.begin();
							vector<collisionHit>::iterator hits_end = hits.end();
							
							for(; hits_iter != hits_end; ++hits_iter)
							{
								vCollisions.push_back(*hits_iter);
							}
						}
					}
				}
	}
	
	orderCollisionHitsByDistance (&vCollisions);
	return vCollisions;
}


bool cCollisionManager::pointVsBox (float x, float y,  collisionBox &cBox)
{
	return pointVsBox (x, y, cBox.xmax, cBox.ymax, cBox.xmin, cBox.ymin);
}

bool cCollisionManager::pointVsBox (float x, float y, float xmax, float ymax, float xmin, float ymin)
{
	if ((x <= xmax) &&
		(x >= xmin) &&
		(y <= ymax) &&
		(y >= ymin))
	{
		return 1;
	}
	return 0;
}


bool cCollisionManager::pointVsCircle(float x, int&y, collisionCircle &cCircle)
{
	return pointVsCircle(x, y, cCircle.posX, cCircle.posY, cCircle.radius);
}

bool cCollisionManager::pointVsCircle(float x, int&y, float circleX, float circleY, float radius)
{
	static float dX = x - circleX;
	static float dY = y - circleY;
	
	return radius * radius > (dX * dX + dY * dY);
/*	static int xmax1 = circleX + radius;
	static int ymax1 = circleY + radius;
	static int xmin1 = circleX - radius;
	static int ymin1 = circleX - radius;
	if (pointVsBox(x,y, xmax1, ymax1, xmin1, ymin1)) //try it as a square test first as an optimization.
	{
		float distance = getDistance(x,y,circleX,circleY);
		if (distance <= radius)
			return true;
	}
	return false;*/
}

bool cCollisionManager::boxVsBox(collisionBox &cBox1, collisionBox &cBox2)
{
	return boxVsBox(cBox1.xmax, cBox1.ymax, cBox1.xmin, cBox1.ymin, cBox2.xmax, cBox2.ymax, cBox2.xmin, cBox2.ymin);
}


bool cCollisionManager::boxVsBox (collisionBox &cBox, float xmax, float ymax, float xmin, float ymin)
{
	return boxVsBox(cBox.xmax, cBox.ymax, cBox.xmin, cBox.ymin,  xmax, ymax, xmin, ymin);
}


bool cCollisionManager::boxVsBox(float xmax1, float ymax1, float xmin1, float ymin1, float xmax2, float ymax2, float xmin2, float ymin2)
{
	//if 2 boxes pass the oneDimentionalTest on both x&y axis there is a collision.
	if ((oneDimentionalTest (xmin1, xmax1, xmin2, xmax2)) &&
		(oneDimentionalTest (ymin1, ymax1, ymin2, ymax2)))
	{
		return 1;
	}
	
	return 0;
}


bool cCollisionManager::boxVsCircle(collisionBox &cBox, collisionCircle &cCircle)
{
	return boxVsCircle(cBox.xmax, cBox.ymax, cBox.xmin, cBox.ymin, cCircle.posX, cCircle.posY, cCircle.radius);
}

bool cCollisionManager::boxVsCircle(float xmax, float ymax, float xmin, float ymin, float circleX, float circleY, float radius)
{
	float rectXRadius = (xmax - xmin)/2;
	float rectYRadius = (ymax - ymin)/2;
	float circleDistanceX = circleX - xmin - rectXRadius;
	if(circleDistanceX < 0) circleDistanceX = -circleDistanceX; //abs() doesn't work on floats in GCC for some reason?
    float circleDistanceY = circleY - ymin - rectYRadius;
	if (circleDistanceY < 0) circleDistanceY = -circleDistanceY;//abs() doesn't work on floats in GCC for some reason?
	
    if (circleDistanceX > (rectXRadius + radius)) { return false; }
    if (circleDistanceY > (rectYRadius + radius)) { return false; }
	
    if ((circleDistanceX <= rectXRadius) && (circleDistanceY <= rectYRadius)) { return true; }
	
    float cornerDistance = sqrt(pow(float(circleDistanceX - rectXRadius),2) +
                          pow(float(circleDistanceY - rectYRadius),2));
	
    return (cornerDistance <= (float)radius);	
}

void cCollisionManager::rayVsBox (float x1, float y1, float x2, float y2, collisionBox &cBox, std::vector<collisionHit> &hits)
{	
	rayVsBox(x1, y1, x2, y2, cBox.xmax, cBox.ymax, cBox.xmin, cBox.ymin, hits);
	
	// add name and var
	if (hits.size())
	{
		vector<collisionHit>::iterator hits_iter = hits.begin();
		vector<collisionHit>::iterator hits_end = hits.end();
		for(; hits_iter != hits_end; ++hits_iter)
		{
			(*hits_iter).name = cBox.name;
			(*hits_iter).var = cBox.var;
		}
	}
}

 void cCollisionManager::rayVsBox (float x1, float y1, float x2, float y2, float xmax, float ymax, float xmin, float ymin, std::vector<collisionHit> &hits)
{

	//get vector of given points [Qx-Px, Qy-Py, Qz-Pz]
	float diffX = float(x2 - x1);
	float diffY = float(y2 - y1);
	
	collisionHit hit;
	
	//get t for each plane so we can determine intersection point  (the three will be equal if they are on the same line)
	//           x – x0   =     y – y0    =    z – z0			 x = x0 + ta
	//   t =     ---------          ---------          ---------			 y = y0 + tb    where    vector = (a, b, c)
    //              a                  b                  c				 z = z0 + tc
	//then test collision with each face of the box.
	float t;
	
	if (diffX != 0)  //protect from divide by 0
	{
		// t corresponds to the distance along the line... 0 being the starting point, 1 being the end.
		// < 0 means the collision occurs before x1, y1 and if t > 1 it means the collision occured past x2, y2 
		t = (xmax - x1) / diffX ;   
		if ((t >= 0)&&(t <= 1))  //you can remove this check if you want to calculate collisions along an infinite line, or remove only one for a vector with a point of origin
		{
			hit.posX = x1 + t * diffX; 
			hit.posY = y1 + t * diffY;
			
			if (pointVsBox(hit.posX, hit.posY, xmax, ymax, xmin, ymin ))
			{	
				// 0 0 -90
				hit.angle = 270;
				hit.distance = t;
				hits.push_back(hit);
			}
		}
		
		
		t = (xmin - x1) / diffX ;
		if ((t >= 0)&&(t <= 1))  
		{
			hit.posX = x1 + t * diffX;
			hit.posY = y1 + t * diffY;
			
			if (pointVsBox(hit.posX, hit.posY, xmax, ymax, xmin, ymin ))
			{
				//0 0 90
				hit.angle = 90;
				hit.distance = t;
				hits.push_back(hit);
			}
		}
	}
	
	if (diffY != 0)  //protect from divide by 0
	{
		t = (ymax - y1) / diffY;
		if ((t >= 0)&&(t <= 1))
		{
			hit.posX = x1 + t * diffX;
			hit.posY = y1 + t * diffY;
			
			if (pointVsBox(hit.posX, hit.posY, xmax, ymax, xmin, ymin ))
			{
				// 0 0 0
				hit.angle  = 180;
				hit.distance = t;
				hits.push_back(hit);
			}
		}
		
		t = (ymin - y1) / diffY;
		if ((t >= 0)&&(t <= 1))
		{
			hit.posX = x1 + t * diffX;
			hit.posY = y1 + t * diffY;
			
			if (pointVsBox(hit.posX, hit.posY, xmax, ymax, xmin, ymin ))
			{
				// 180 0 0
				hit.angle  = 0;
				hit.distance = t;
				hits.push_back(hit);
			}
		}
	}
	
	//TODO still must add box name and vars after the return, as only the vectors and coords have been added
}

bool cCollisionManager::oneDimentionalTest( float min1, float max1,  float min2, float max2)
{
	
	if ((min1 >= min2) && (min1 <= max2))
		return 1;
	if ((max1 >= min2) && (max1 <= max2))
		return 1;
	
	return 0;
}


std::vector<collisionHit> cCollisionManager::rayVsCircle (float x1, float y1,  float x2, float y2, collisionCircle &cCircle)
{
	return rayVsCircle(x1, y1, x2, y2, cCircle.posX, cCircle.posY, cCircle.radius);
}

std::vector<collisionHit> cCollisionManager::rayVsCircle (float x1, float y1,  float x2, float y2, float circleX, float circleY, float radius)
{
	std::vector<collisionHit> vCollisions;

	float diffX = x2 - x1;
	float diffY = y2 - y1;
	if ((diffX == 0) && (diffY == 0))
	{
		// A and B are the same points, no way to calculate intersection
		return vCollisions;
	}
	//TODO

	return vCollisions;
}


bool cCollisionManager::circleVsCircle(collisionCircle &cCircle1, collisionCircle &cCircle2)
{
	return circleVsCircle(cCircle1.posX, cCircle1.posY, cCircle1.radius, cCircle2.posX, cCircle2.posY, cCircle2.radius);
}

bool cCollisionManager::circleVsCircle(collisionCircle &cCircle1, float circleX, float circleY, float radius)
{
	return circleVsCircle(cCircle1.posX, cCircle1.posY, cCircle1.radius, circleX, circleY, radius);
}

bool cCollisionManager::circleVsCircle(float circle1X, float circle1Y, float radius1, float circle2X, float circle2Y, float radius2)
{
	float a = radius1 + radius2;
	float dX = circle1X - circle2X;
	float dY = circle1Y - circle2Y;
	
	return a * a > (dX * dX + dY * dY);
}


collisionBox cCollisionManager::rayToBox(float x1, float y1, float x2, float y2 )
{
	collisionBox cBox;
	
	if (x1 > x2)
	{
		cBox.xmax = x1;
		cBox.xmin = x2;
	}
	else
	{
		cBox.xmax = x2;
		cBox.xmin = x1;
	}
	
	if (y1 > y2)
	{
		cBox.ymax = y1;
		cBox.ymin = y2;
	}
	else
	{
		cBox.ymax = y2;
		cBox.ymin = y1;
	}
	
	return cBox;
}


void cCollisionManager::orderCollisionHitsByDistance(std::vector<collisionHit> *hits)
{
	//if we are using t as distance, this will only work properly when comparing hits generated from the same line calculation.	
	if (hits->size())
	{
		std::vector<collisionHit> hitsTemp = (*hits);	
		hits->clear();
		collisionHit largest;		
		int lastLargestIterator;
		
		int arraySize = hitsTemp.size();
		for (int i = 0; i < arraySize ; i ++)
		{
			largest.distance = 0;
			for (int j = 0; j < arraySize ; j ++)
			{
				if ((hitsTemp[j].distance >= largest.distance) &&
					(hitsTemp[j].distance >= 0))
				{
					largest = hitsTemp[j];
					lastLargestIterator = j;
				}
			}
			(*hits).push_back(largest);
			hitsTemp[lastLargestIterator].distance = -1;
		}
	}
}

float cCollisionManager::getDistance(float x1, float y1, float x2, float y2)
{	
	return sqrt( pow(float(x2 - x1),2) +  pow(float(y2 - y1),2) );  //distance in units
}

/*
string cCollisionManager::getAttrib(TiXmlElement *XMLNode, const string &attrib, const string &defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return XMLNode->Attribute(attrib.c_str());
	else
		return defaultValue;
}

Real cCollisionManager::getAttribReal(TiXmlElement *XMLNode, const string &attrib, Real defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return StringConverter::parseReal(XMLNode->Attribute(attrib.c_str()));
	else
		return defaultValue;
}*/



 //distance:
 //   a^2 = x^2 + y^2 + z^2
 