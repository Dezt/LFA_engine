#include "miniGUI.h"

using namespace miniGUI;
using namespace LFA;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    **  UTILITY  CLASSES   **   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

buttonPosition::buttonPosition()
{
	usingRelative = false;
	data.abs.left = 0;
	data.abs.top = 0;
}

buttonPosition::buttonPosition(const relativePosition &relPosition, short offsetLeft, short offsetTop)
{
	usingRelative = true;
	data.rel.position = relPosition;
	data.rel.x = offsetLeft;
	data.rel.y = offsetTop;
}

buttonPosition::buttonPosition(short absoluteLeft, short absoluteTop)
{
	usingRelative = false;
	data.abs.left = absoluteLeft;
	data.abs.top = absoluteTop;
}


buttonScheme::buttonScheme(float u, float v, float w, float h) :
BLANK(false)
{
	mouseOff_coords = mouseOver_coords = onClick_coords = onRelease_coords = LFA::FloatCoords(u, v, w, h);
	onClick_color = onRelease_color = mouseOver_color = mouseOff_color = LFA::Color(1.0f, 1.0f, 1.0f);

}
buttonScheme::buttonScheme(FloatCoords coords) :
BLANK(false)
{
	mouseOff_coords = mouseOver_coords = onClick_coords = onRelease_coords = coords;
	onClick_color = onRelease_color = mouseOver_color = mouseOff_color = LFA::Color(1.0f, 1.0f, 1.0f);
}
buttonScheme::buttonScheme() :
BLANK(true)
{
	mouseOff_coords = mouseOver_coords = onClick_coords = onRelease_coords = LFA::FloatCoords(0,0,0,0);
	onClick_color = onRelease_color = mouseOver_color = mouseOff_color = LFA::Color(1.0f, 1.0f, 1.0f, 0.0f);
}
buttonScheme::~buttonScheme(void)
{}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    **  BUTTON  CLASS    **   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

button::button(std::string &buttonName, const buttonPosition &position, buttonManager * mgr, button * parentButton, const buttonScheme &style, bool isActiveButton) :
name(buttonName),
buttonMgr(mgr),
q(NULL),
rotation(0.0f),
parent(parentButton),
movable(false),  //buttons default to non-movable.
centerOnGrab(true),
isMouseOver(0),
style(style),
suppressed(false),
state(true),
position(position),
isActive(isActiveButton),
trigger_onClick(true),			//if you would like events to be sent out or suppressed by default you can set that behavior here
trigger_onRelease(true),
trigger_mouseOver(true),
trigger_mouseOff(true),
useLimits(false),
x1Limit(0),
x2Limit(0),
y1Limit(0),
y2Limit(0)
{

	if (parent)
	{
		if (parent->suppressed)
			suppressed = true;
	}

	fadingOut = false;
	fadingOutStart = fadingOutEnd = 0;
	fadingIn = false;
	fadingInStart = fadingInEnd = 0;

	//this stuff is commented out because the cursor button is different, so this is done by the manager upon button creation
	//	mgr->buttons.push_back(this);  //it is important that the button is added to the main button list BEFORE trying to zOrderc.
	//	resetButton(true);	
	//	setSize(width, height);	
}
button::~button(void)
{}

button * button::createChildButton( std::string name, relativePosition position, int x, int y, const buttonScheme &scheme, bool isActiveButton /*= true*/ )
{
	return createChildButton(name, position, x, y, (int)scheme.mouseOff_coords.width, (int)scheme.mouseOff_coords.height, scheme, isActiveButton);
}
button * button::createChildButton(std::string name, relativePosition position, int x, int y, short  width, short  height, bool isActiveButton)
{
	return createChildButton(name, position, x, y, width, height, style, isActiveButton);
}
button * button::createChildButton(std::string name, relativePosition position, int x, int y, short  width, short  height, const buttonScheme &scheme, bool isActiveButton)
{
	buttonPosition bp(position, x, y);
	button *b = buttonMgr->createButton(name, bp, width, height, scheme, 0, isActiveButton);
	b->parent = this;
	b->resetPosition(); //will have been placed wrongly, not knowing it was a child.

	childButtons.push_back(b);

	return b;
}


button * button::getChildButton(std::string name)
{
	std::string currentButtonName;
	for ( buttonItr = childButtons.begin(); buttonItr != childButtons.end(); ++buttonItr )
	{
		currentButtonName = *(*buttonItr)->getName();
		if ( currentButtonName == name)
			return (*buttonItr);
	}
	return NULL;  //didn't find a button with the given name.
}


void button::cascadeHide( void )
{
	if (q)
	{
		buttonMgr->mQuads->removeQuad(q);
		q = NULL;
	}
	std::vector<button*>::iterator itr;
	for ( itr = childButtons.begin(); itr != childButtons.end(); ++itr )
	{
		(*itr)->cascadeHide();
	}
}

void button::cascadeShow( void )
{
	if (!state)
		return;
	if (suppressed)
		return;

	if (!q) //make sure quad exists before editing.
	{
		if (!style.BLANK)
		{		
			q = buttonMgr->mQuads->createQuad();
			buttonMgr->reorderQuads();
		}
	}
	std::vector<button*>::iterator itr;
	for ( itr = childButtons.begin(); itr != childButtons.end(); ++itr )
	{
		(*itr)->cascadeShow();
	}
}

button *  button::hide(bool fade, unsigned short fadeDurationMS)
{
	if(!state) 
		return this;
	setSuppressed(true);  //keep  children from sending events.
	state = false;


	if(fadingOut)	//already busy hiding.
		return this;

	if(fadingIn)
		fadingInStart = fadingInEnd = fadingOutStart = fadingOutEnd = fadingIn = fadingOut = 0;

	if(fade)
	{
		fadingOutStart = utils.getTime();
		fadingOutEnd = fadingOutStart + fadeDurationMS + 1;
		fadingOut = true;
	}
	else
	{
		cascadeHide(); //removing the quad does the actual hiding.
	}

	return this;
}

button *  button::show(bool fade, unsigned short fadeDurationMS)
{	
	if(state) //already showing
		return this;
	state = true;
	setSuppressed(false); //allow sending messages again.

	cascadeShow();

	if(fadingIn || fadingOut)
		fadingInStart = fadingInEnd = fadingOutStart = fadingOutEnd = fadingIn = fadingOut = 0;

	if(fade)
	{
		fadingInStart = utils.getTime();
		fadingInEnd = fadingInStart + fadeDurationMS + 1;
		fadingIn = true;
		setOpacity(0);	//clear any half-transparency action from past events so it can fade from pure nothing.
	}
	else
		setOpacity(1.0f);

	return this;
}

void button::setSuppressed(bool onOff)
{
	if (state)  //keep from cascading if we have been directly hidden.
	{
		//NOTE:  suppressed is only activated on children,  not the button itself
		std::vector<button*>::iterator itr;
		for ( itr = childButtons.begin(); itr != childButtons.end(); ++itr )
		{
			(*itr)->suppressed = onOff;
			(*itr)->setSuppressed(onOff); //allow children to send events again.
		}
	}
}

bool button::getVisibility(void)
{
	if (q)
		return true;
	if (!state)
		return false;
	if (suppressed)
		return false;

	return true;
}

button * button::setButtonScheme( buttonScheme & scheme )
{
	return setButtonScheme(scheme, true); // this setButtonScheme is coming from public,  therefore it must be adjusted to make it intuitive.
}
button * button::setButtonScheme( buttonScheme & scheme, bool adjust )
{
	style = scheme;

	if (adjust)
		makeSchemeIntuitive(style);

	// update button
	if (style.BLANK) //delete the quad
	{	
		if(q)
		{
			buttonMgr->mQuads->removeQuad(q);
			q = NULL;
		}
	}
	else 
	{

		if (q)
		{
			q->setPixelTextureCoords(style.mouseOff_coords);
			q->setColor(style.mouseOff_color);
		}
		else if (!suppressed && state)
		{
			q = buttonMgr->mQuads->createQuad();
			buttonMgr->reorderQuads();
		}
		resetPosition();
	}

	return this;
}



bool button::update(void)
{

	if(fadingIn)
	{
		unsigned long time = utils.getTime();
		float fadeMod = 1;
		if(fadingInEnd < time)
		{
			fadingInStart = fadingInEnd = fadingIn = 0;
			setOpacity(1);
		}
		else
		{
			fadeMod = (float)(time - fadingInStart) / (float)(fadingInEnd - fadingInStart);
			setOpacity(fadeMod);
		}
		return true;
	} 
	else if(fadingOut)
	{
		unsigned long time = utils.getTime();
		if(fadingOutEnd < time)
		{
			fadingOutStart = fadingOutEnd = 0;
			fadingOut = false;	
			cascadeHide();
		}
		else
		{
			float fadeMod = 1 - (float)(time - fadingOutStart) / (float)(fadingOutEnd - fadingOutStart);
			setOpacity(fadeMod);
		}
		return true;
	}
	return false;
}


button *  button::setOpacity(float opacity)
{
	//do not check for panel visibility here so that you can still set the opacity of something to prepare it for showing at half opacity, for example.

	//If there is no quad and it should be shown - give us a quad and set it up.  
	//This should not be contained in show() or hide() because show/hide should not be recursive for 'state' to work properly.
	if ((!style.BLANK)&&(!suppressed)&&(!q)&&(state))  
		resetButton();

	if (q) 
	{
		Color c = style.mouseOff_color;
		if (opacity < style.mouseOff_color.a)  //this if statement keeps setOpacity from overriding the style's opacity.
			c.a = opacity;
		else
			c.a = style.mouseOff_color.a;
		q->setColor(c);
	}

	//adjust all child buttons to inherit this opacity
	for ( buttonItr = childButtons.begin(); buttonItr != childButtons.end(); ++buttonItr )
	{
		(*buttonItr)->setOpacity(opacity);
	}


	return this;
}

button * button::setColor(const LFA::Color &clr, bool cascadeToScheme)
{
	if (q) 
		q->setColor(clr);


	if (cascadeToScheme)
	{
		style.mouseOff_color = clr;
		style.mouseOver_color = clr;
		style.onClick_color = clr;
		style.onRelease_color = clr;
	}
	
	return this;
}

button * miniGUI::button::setColor( const LFA::Color &upperLeft, const LFA::Color &uppeRight, const LFA::Color &lowerLeft, const LFA::Color &lowerRight )
{
	if (q) 
	{
		q->setTLColor(upperLeft);
		q->setTRColor(uppeRight);
		q->setBLColor(lowerLeft);
		q->setBRColor(lowerRight);
	}
	return this;
}

button * miniGUI::button::getLimitOffset( int offset[2] )
{
	if (useLimits) //check if limits are even imposed on this button
	{
		if (position.usingRelative)
		{
			//int diffX = x1Limit - x2Limit;
			//int posX = position.data.rel.x - x1Limit;

			//percent[0] = posX/diffX;

			//int diffY = y1Limit - y2Limit;
			//int posY = position.data.rel.y - y1Limit;

			//percent[1] = posY/diffY;

			offset[0] = position.data.rel.x - x1Limit;
			offset[1] = position.data.rel.y - y1Limit;

		}
		else
		{

			//int diffX = x1Limit - x2Limit;
			//int posX = position.data.abs.left - x1Limit;

			//percent[0] = posX/diffX;

			//int diffY = y1Limit - y2Limit;
			//int posY = position.data.abs.top - y1Limit;

			//percent[1] = posY/diffY;

			offset[0] = position.data.abs.left - x1Limit;
			offset[1] = position.data.abs.top - y1Limit;
		}
	}	

	return this;
}


button * button::setPosition( int posX, int posY )
{
	buttonPosition bp(posX, posY);
	return setPosition(bp);
}

button *  button::setPosition(buttonPosition &p)
{
	if (useLimits) //check if limits are even imposed on this button
	{
		if (p.usingRelative)
		{
			if (p.data.rel.x < x1Limit) 
				p.data.rel.x = x1Limit;
			if (p.data.rel.x  > x2Limit)
				p.data.rel.x = x2Limit;
			if (p.data.rel.y < y1Limit) 
				p.data.rel.y = y1Limit;
			if (p.data.rel.y  > y2Limit)
				p.data.rel.y = y2Limit;
		}
		else
		{
			if (p.data.abs.left < x1Limit) 
				p.data.abs.left= x1Limit;
			if (p.data.abs.left  > x2Limit)
				p.data.abs.left = x2Limit;
			if (p.data.abs.top < y1Limit) 
				p.data.abs.top = y1Limit;
			if (p.data.abs.top  > y2Limit)
				p.data.abs.top = y2Limit;
		}
	}	

	position = p;

	if(position.usingRelative)
	{
		FloatCoords parentCoords;
		if (parent) 
			parentCoords = parent->pixelScreenCoords;

		int winWidth = buttonMgr->screenResX;
		int winHeight = buttonMgr->screenResY;
		int width = (int)pixelScreenCoords.width;
		int height = (int)pixelScreenCoords.height;

		int left = position.data.rel.x;
		if (parent) left =  position.data.rel.x + (int)parentCoords.x ;
		int center = (winWidth/2)-(width/2) + position.data.rel.x;
		if (parent)	center = (int)(parentCoords.width/2)-(width/2) + position.data.rel.x + (int)parentCoords.x ; //if this is a child, this should be relative to parent.
		int right = winWidth - width - position.data.rel.x;
		if (parent)	right = (int)parentCoords.width - width - position.data.rel.x + (int)parentCoords.x; //if this is a child, this should be relative to parent.

		int top = position.data.rel.y;
		if (parent) top =  position.data.rel.y + (int)parentCoords.y;
		int middle = (winHeight/2)-(height/2) + position.data.rel.y;
		if (parent)	middle = (int)(parentCoords.height/2)-(height/2) + position.data.rel.y + (int)parentCoords.y; //if this is a child, this should be relative to parent.
		int bottom = winHeight - height - position.data.rel.y;
		if (parent)	bottom = (int)parentCoords.height - height - position.data.rel.y + (int)parentCoords.y; //if this is a child, this should be relative to parent.

		switch(position.data.rel.position)
		{
		case LEFT:
			pixelScreenCoords.x = (float)left; 
			pixelScreenCoords.y = (float)middle;
			break;
		case TOP_LEFT:
			pixelScreenCoords.x = (float)left;
			pixelScreenCoords.y = (float)top;
			break;
		case TOP_CENTER:
			pixelScreenCoords.x = (float)center;
			pixelScreenCoords.y = (float)top;
			break;
		case TOP_RIGHT:
			pixelScreenCoords.x = (float)right;
			pixelScreenCoords.y = (float)top;
			break;
		case RIGHT:
			pixelScreenCoords.x = (float)right;
			pixelScreenCoords.y = (float)middle;
			break;
		case BOTTOM_RIGHT:
			pixelScreenCoords.x = (float)right;
			pixelScreenCoords.y = (float)bottom;
			break;
		case BOTTOM_CENTER:
			pixelScreenCoords.x = (float)center;
			pixelScreenCoords.y = (float)bottom;
			break;
		case BOTTOM_LEFT:
			pixelScreenCoords.x = (float)left;
			pixelScreenCoords.y = (float)bottom;
			break;
		case CENTER:
			pixelScreenCoords.x = (float)center;
			pixelScreenCoords.y = (float)middle;
			break;
		default:
			pixelScreenCoords.x = (float)position.data.rel.x;
			pixelScreenCoords.y = (float)position.data.rel.y;
			break;
		}
	}
	else	//using absolute position 
	{
		pixelScreenCoords.x = position.data.abs.left;
		pixelScreenCoords.y = position.data.abs.top;

		if (parent) 
		{
			FloatCoords parentCoords = parent->pixelScreenCoords;

			pixelScreenCoords.x += parentCoords.x;
			pixelScreenCoords.y += parentCoords.y;
		}		
	}
	if (q)
		q->setPixelScreenCoords(pixelScreenCoords);

	std::vector<button*>::iterator itr;
	for ( itr = childButtons.begin(); itr != childButtons.end(); ++itr )//reposition all its children which may be positioned relatively inside the button.
	{
		(*itr)->resetPosition();
	}
	return this;
}

button * button::offsetPositionToScreenCoord( int posX, int posY )
{	
	buttonPosition pos = getAbsolutePosition();
	int offsetX = posX - pos.data.abs.left;
	int offsetY = posY - pos.data.abs.top;

	if (position.usingRelative)
	{
		switch(position.data.rel.position)
		{
		case LEFT:
			position.data.rel.x += offsetX ; 
			position.data.rel.y += offsetY ;
			break;
		case TOP_LEFT:
			position.data.rel.x += offsetX ; 
			position.data.rel.y += offsetY ;
			break;
		case TOP_CENTER:
			position.data.rel.x += offsetX; 
			position.data.rel.y += offsetY;
			break;
		case TOP_RIGHT:
			position.data.rel.x -= offsetX;
			position.data.rel.y += offsetY;
			break;
		case RIGHT:
			position.data.rel.x -= offsetX; 
			position.data.rel.y += offsetY;
			break;
		case BOTTOM_RIGHT:
			position.data.rel.x -= offsetX;
			position.data.rel.y -= offsetY;
			break;
		case BOTTOM_CENTER:
			position.data.rel.x += offsetX ;  
			position.data.rel.y -= offsetY ;
			break;
		case BOTTOM_LEFT:
			position.data.rel.x += offsetX ; //checked
			position.data.rel.y -= offsetY ;
			break;
		case CENTER:
			position.data.rel.x += offsetX; 
			position.data.rel.y += offsetY;
			break;
		default:
			position.data.rel.x += offsetX; 
			position.data.rel.y += offsetY;
			break;
		}
	}
	else //using absolute positioning
	{
		position.data.abs.top +=  offsetY;
		position.data.abs.left +=  offsetX;
	}

	resetPosition();
	return this;
}

void button::resetPosition(void)
{
	setPosition(position);
}

void button::resetButton( bool adjustScheme )
{
	setButtonScheme(style, adjustScheme);
	//	setSize((int)pixelScreenCoords.width, (int)pixelScreenCoords.height);
	//setPosition(position);
	if (q)
		q->setRotation(rotation);
}

void button::makeSchemeIntuitive( buttonScheme & scheme )
{
	scheme.mouseOff_coords.y = -scheme.mouseOff_coords.y -  scheme.mouseOff_coords.height;
	scheme.mouseOver_coords.y = -scheme.mouseOver_coords.y - scheme.mouseOver_coords.height;
	scheme.onClick_coords.y = -scheme.onClick_coords.y - scheme.onClick_coords.height;
	scheme.onRelease_coords.y = -scheme.onRelease_coords.y - scheme.onRelease_coords.height;
}


const buttonPosition * button::getPosition(void)
{
	return &position;
}

buttonPosition button::getAbsolutePosition(void)
{
	int top = (int)pixelScreenCoords.y;
	int left = (int)pixelScreenCoords.x;

	return buttonPosition(left,top);
}

button * button::getAbsolutePosition( float pos[2] )
{
	pos[0] = pixelScreenCoords.x;
	pos[1] = pixelScreenCoords.y;
	return this;
}


button * button::setLimits(bool useLimits)
{
	this->useLimits = useLimits;
	return this;
}

button * button::setLimits(short int x1, short int y1, short int x2, short int y2, bool useLimits)
{
	this->useLimits = useLimits;

	x1Limit = x1;
	x2Limit = x2;
	y1Limit = y1;
	y2Limit = y2;

	return this;
}


button *  button::setSize(short unsigned int width, short unsigned int height)
{
	pixelScreenCoords.width = width;
	pixelScreenCoords.height = height;

	resetPosition(); //will update this button and any children as well

	return this;
}


button *  button::setTrigger(buttonAction action, bool onOff)
{
	if (action == ONCLICK)
		trigger_onClick = onOff;
	else if (action == ONRELEASE)
		trigger_onRelease = onOff;
	else if (action == MOUSEOVER)
		trigger_mouseOver = onOff;
	else if (action == MOUSEOFF)
		trigger_mouseOff = onOff;

	return this;
}

bool button::getTrigger(buttonAction action)
{
	if (action == ONCLICK)
		return trigger_onClick;
	else if (action == ONRELEASE)
		return trigger_onRelease;
	else if (action == MOUSEOVER)
		return trigger_mouseOver;
	else if (action == MOUSEOFF)
		return trigger_mouseOff;

	return 0;
}



void button::setParent(std::string newParentName)
{
	button * newParent = buttonMgr->getButton(newParentName);
	setParent(newParent);
}

void button::setParent(button * newParentButton)
{
	if (name == "CURSOR")	//do not allow reparenting of the cursor button,  this is unsupported. (ie there is no interface to parent it to its own overlay again... .)
		return;

	if (newParentButton)
	{
		if (parent) 	//this is a child of another button
		{
			//remove ourselves from the list that our parent keeps of its child buttons
			for ( buttonItr = parent->childButtons.begin(); buttonItr != parent->childButtons.end(); ++buttonItr )
			{
				if((*buttonItr) == this)
				{
					parent->childButtons.erase(buttonItr);
					break;
				}
			}
		}

		parent = newParentButton;
		//add ourselves to our the list of our new parent
		parent->childButtons.push_back(this);
	}
}

button * button::setZOrder(short unsigned int z)
{
	zOrder = z;
	buttonMgr->reorderQuads();
	return this;
}

bool button::hitTest(int x, int y)
{
	if(!isActive)
		return false;
	if(!state) 
		return false;
	if(suppressed)
		return false;
	if(fadingOut)
		return false;

	float right = pixelScreenCoords.x + pixelScreenCoords.width; 
	float bottom = pixelScreenCoords.y + pixelScreenCoords.height;

	if ((pixelScreenCoords.x <= x ) &&
		(x <= right) &&
		(pixelScreenCoords.y <= y) &&
		(y <= bottom))
	{
		return 1;
	}
	return 0;
}

bool button::onClick(void)
{
	if (q)
	{
		q->setPixelTextureCoords(style.onClick_coords);
		q->setColor(style.onClick_color); //this also clears any half transparencies from past events.
	}

	return trigger_onClick;
}

bool button::onRelease()
{
	if (q)
	{
		q->setPixelTextureCoords(style.onRelease_coords);
		q->setColor(style.onRelease_color); //this also clears any half transparencies from past events.
	}

	return trigger_onRelease;
}

bool button::mouseOver()
{
	if (q)
	{
		q->setPixelTextureCoords(style.mouseOver_coords);
		q->setColor(style.mouseOver_color); //this also clears any half transparencies from past events.
	}

	isMouseOver = 1;
	return trigger_mouseOver;
}
bool button::mouseOff()
{
	if (q)
	{
		q->setPixelTextureCoords(style.mouseOff_coords);
		q->setColor(style.mouseOff_color); //this also clears any half transparencies from past events.
	}

	isMouseOver = 0;
	return trigger_mouseOff;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    **  BUTTON  MANAGER CLASS    **   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

buttonManager::buttonManager(std::string buttonImagesTextureFile, buttonScheme defaultButtonScheme, unsigned int screenWidth, unsigned int screenHeight) :
mQuads(new QuadSet(screenWidth, screenHeight)),
defaultButtonScheme(defaultButtonScheme),
screenResX(screenWidth),
screenResY(screenHeight),
grabbedButton(NULL),
cursorButton(NULL),
grabbingMouseButton(1),  //defnes the mouse button that is used to drag buttons around
zOrderCounter(5),
eventContainer(ONCLICK, -1, NULL, NULL)  //junk event container just for the initialization
{
	mQuads->beginEdit();
	mQuads->setTextureImage(utils.getAssetPath(buttonImagesTextureFile,"tga"));
	mQuads->setTextureFiltering(FILTERING_NONE);
}

buttonManager::~buttonManager(void)
{
	delete mQuads;
}

void buttonManager::resetScreenResolution(unsigned int x, unsigned int y)
{
	screenResX= x;
	screenResY = y;
	mQuads->windowResized(x, y);

	//reposition all elements
	for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
	{
		(*buttonItr)->resetPosition();
	}
}



button * buttonManager::setCursor(FloatCoords &texCoords,  unsigned short hotspotX,  unsigned short hotspotY, bool visibility)
{
	if (!cursorButton)
	{
		buttonPosition bp(0,0);
		std::string s("CURSOR");
		cursorButton = new button(s, bp, this,  NULL, defaultButtonScheme, false);	//create a button that represents  the cursor. 
		cursorButton->setZOrder(60000);
	}

#ifdef WINDOWS_BUILD
	ShowCursor(false);  //hide windows cursor
#endif
	mouseOffsetX = hotspotX;
	mouseOffsetY = hotspotY;

	buttonScheme bs(texCoords);
	cursorButton->setButtonScheme(bs);
	cursorButton->setSize((unsigned short)texCoords.width,(unsigned short)texCoords.height);

	cursorButton->setPosition(mouseX - mouseOffsetX, mouseY- mouseOffsetY);

	if (visibility)
		cursorButton->show(false);
	else
		cursorButton->hide(false);

	return cursorButton;
}

button * buttonManager::showCursor(void)
{
	if (cursorButton)
		cursorButton->show(false);
	else return NULL;
	return cursorButton;
}
button * buttonManager::hideCursor(void)
{
	if (cursorButton)
		cursorButton->hide(false);
	else return NULL;
	return cursorButton;
}



button * buttonManager::getTopButton(void)
{
	std::vector<button*>hits;  //determine who is on top and run the command only on him.
	for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
	{
		if ((*buttonItr)->hitTest(mouseX,mouseY))
		{
			hits.push_back((*buttonItr));  //keep track of positives
		}
	}

	//find out who has the highest zOrder ie who gets the onClick event 
	button* topButton = NULL;
	unsigned short topValue = 0;
	for ( buttonItr = hits.begin(); buttonItr != hits.end(); ++buttonItr )
	{		
		if ((*buttonItr)->zOrder >= topValue)
		{
			if((*buttonItr) != grabbedButton)  //skip the dragged button,  we already know what it is and we want to click and drop things on buttons other than him. 
			{
				topButton = (*buttonItr);  //closest so far
				topValue = (*buttonItr)->zOrder; //move up the topValue
			}
		}
	}

	return topButton;
}


bool miniGUI::buttonManager::injectGrabbedButton( button * b )
{
	if (b) 
	{
		grabbedButton = b;
		injectMouseMove(mouseX, mouseY); // force a move to the same location so that the button will snap to the mouse
		return true;
	}
	return false;
}

bool buttonManager::injectMouseDown(int mouseButttonIndex)
{

	button* topButton = getTopButton();
	

	if (topButton)
	{
		if(topButton->onClick()) //register the event in the button and check the returned trigger
			eventLog.push_back(buttonEvent(ONCLICK, mouseButttonIndex, topButton)); //log the event


		if (mouseButttonIndex == grabbingMouseButton)	//should we check for movable buttons?
		{
			if (topButton->getMovable())
			{
				injectGrabbedButton(topButton);
			}
		}			
	}
	else
		return false;

	return true;
}

bool buttonManager::injectMouseUp(int mouseButttonIndex)
{
	button* topButton = getTopButton();

	if (topButton)
	{
		if (topButton->onRelease())
		{
			std::string message;
			//if a button is currently grabbed,  pass its name as the 'additional message' because that means that that button has been drag/dropped onto this one.
			if (grabbedButton)
				eventLog.push_back(buttonEvent(ONRELEASE,mouseButttonIndex, topButton, grabbedButton));  //log the event
			else 
				eventLog.push_back(buttonEvent(ONRELEASE,mouseButttonIndex, topButton));  //log the event
		}
	}

	if (mouseButttonIndex == grabbingMouseButton)
	{
		if (grabbedButton) // we are dropping a button.
		{
			grabbedButton->onRelease();
			if (!topButton)  //send an event that the dragged button was released on nothing.
				eventLog.push_back(buttonEvent(ONRELEASE,mouseButttonIndex, NULL, grabbedButton)); 
		}
		grabbedButton = NULL;		//stop carrying any grabbed button
	}

	return true;
}


bool buttonManager::injectMouseMove(int xPos, int yPos)
{
	//drag the grabbed button (if any)
	if (grabbedButton)
	{	
		if (grabbedButton->centerOnGrab )//this centers the grabbed button to the mouse
		{
			int offsetX = (int)grabbedButton->pixelScreenCoords.width/2;  
			int offsetY = (int)grabbedButton->pixelScreenCoords.height/2;
			grabbedButton->offsetPositionToScreenCoord(xPos - offsetX, yPos - offsetY);
		}
		else	//do a relative drag.
		{
			buttonPosition p = grabbedButton->getAbsolutePosition();
			grabbedButton->offsetPositionToScreenCoord(p.data.abs.left +  xPos - mouseX, p.data.abs.top + yPos - mouseY);  //current pos + old mousePos - new mousePos
		}
	}

	mouseX = xPos;  
	mouseY = yPos;
	if (cursorButton)
		cursorButton->setPosition(xPos - mouseOffsetX, yPos - mouseOffsetY);  //update our cursor, line it up with trigger point.

	//  trigger all the mouseOvers and mouseOffs
	for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
	{
		if  ((*buttonItr)->hitTest(mouseX,mouseY)) //are we over x button?
		{
			if (!(*buttonItr)->isMouseOver)  //if the mouse was previously off,  trigger the mouse over.
			{
				if((*buttonItr)->mouseOver()) //register the event and check the returned trigger
					eventLog.push_back(buttonEvent(MOUSEOVER, -1, (*buttonItr)));
			}
		}
		else
		{
			if ((*buttonItr)->isMouseOver)  //if the mouse was previously over,  trigger the mouse off.
			{
				if((*buttonItr)->mouseOff()) //register the event and check the returned trigger
					eventLog.push_back(buttonEvent(MOUSEOFF,-1, (*buttonItr)));
			}
		}
	}

	return true;
}

bool buttonManager::injectMouseWheel(short int z)
{
	if (z != 0)
	{
		button* topButton = getTopButton();
		if (topButton)
		{
			buttonAction a;
			if (z > 0)
				a = MOUSEWHEELUP;
			else
				a = MOUSEWHEELDOWN;

			eventLog.push_back(buttonEvent(a, -1, topButton));
		}
	}
	return true;
}


bool buttonManager::forceMove(std::string &buttonName, short x, short y)
{
	button * b = getButton(buttonName);
	if (!b)
		return false;

	buttonPosition pos = b->getAbsolutePosition();
	injectMouseMove(pos.data.abs.left + x, pos.data.abs.top + y); //position mouse

	return true;
}

void buttonManager::forceClick(int mouseButttonIndex)
{
	injectMouseDown(mouseButttonIndex); //insert a single click
	injectMouseUp(mouseButttonIndex);
}

void buttonManager::forceClickHidden(std::string buttonName)
{
	//force in the event.
	button * b = getButton(buttonName);
	eventLog.push_back(buttonEvent(ONCLICK, 0, b)); //log the event
	eventLog.push_back(buttonEvent(ONRELEASE, 0, b));
}


button* buttonManager::createButton( std::string name, const buttonPosition &position, const buttonScheme &scheme, unsigned short zOrder /*= 0*/, bool isActive /*= true*/ )
{
	return createButton(name, position, (unsigned short)scheme.mouseOff_coords.width, (unsigned short)scheme.mouseOff_coords.height, scheme, zOrder, isActive);
}

button* buttonManager::createButton(std::string buttonName,  const buttonPosition &position, unsigned short width, unsigned short height, unsigned short zOrder, bool isActive)
{
	return createButton(buttonName, position, width, height, defaultButtonScheme, zOrder, isActive);
}

button* buttonManager::createButton(std::string buttonName,  const buttonPosition &position, unsigned short width, unsigned short height, const buttonScheme &scheme, unsigned short zOrder,  bool isActive)
{
	if(!zOrder)
		zOrder = zOrderCounter++;

	button * b = new button(buttonName, position, this, NULL, scheme, isActive);
	buttons.push_back(b);  
	b->setZOrder(zOrder);
	b->resetButton(true);	
	b->setSize(width, height);	

	return b;  //return our new button which is at the end of the vector
}


void buttonManager::setDefaults(buttonScheme &scheme)
{
	defaultButtonScheme = scheme;
}

buttonEvent * buttonManager::getEvent(void)
{
	//basically,  this is a hopper system that you can get the events from one by one
	if (eventLog.empty())
		return NULL;
	else
	{
		eventContainer = eventLog.back();
		eventLog.pop_back();
	}
	return &eventContainer;
}

button * buttonManager::getButton(std::string buttonName)
{
	std::string currentButtonName;
	for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
	{
		if ( *(*buttonItr)->getName() == buttonName)
			return (*buttonItr);
	}

	return NULL;  //didn't find a button with the given name.
}


void buttonManager::deleteButton(std::string buttonName)
{
	button * b = getButton(buttonName);
	deleteButton(b);
}

void buttonManager::deleteButton(button * b)
{
	//this function is a little dangerous... lets make sure none of the buttons being deleted are being pointed to by these...

	if (grabbedButton)
		grabbedButton = NULL;

	if(b)	//this is a recursive algorithm,  so i just want to be safe and make sure b hasnt been deleted already if someone decides to call this inside some kind of loop
	{
		//delete children first.
		std::vector<button *>children = b->childButtons;  // we need to make a copy of the vector so that when we delete children so they dont muck with the list we are iterating on while we are deleting them.
		std::vector<button*>::iterator itr; //don't use the buttonManager button vector iterator here because this function is recursive.
		for ( itr = children.begin(); itr != children.end(); ++itr )
		{
			deleteButton((*itr));
		}

		button * parent = b->parent;
		if (parent)
		{
			//remove the button from the list that its parent keeps of its child buttons			
			for ( itr = parent->childButtons.begin(); itr != parent->childButtons.end(); ++itr )
			{
				if((*itr) == b)
				{
					parent->childButtons.erase(itr);
					break;
				}
			}
			//delete the quad
			if (b->q)
				mQuads->removeQuad(b->q);
			b->q = NULL;
		}

		//remove button from the master list.
		for ( itr = buttons.begin(); itr != buttons.end(); ++itr )
		{
			if((*itr) == b)
			{
				buttons.erase(itr);
				break;
			}
		}
		//delete the button instance
		delete b;
		b = NULL;
	}
}

void buttonManager::deleteAllButtons(void)
{

	for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
	{
		deleteButton((*buttonItr));
	}

}

void buttonManager::shutdown(void)
{
	deleteAllButtons();
	deleteButton(cursorButton);
	mQuads->removeAllQuads(); //recursive safety
}


void buttonManager::reorderQuads( void )
{
	//gather the visible buttons only
	std::vector<button *> buttonList;
	for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
	{
		if ((*buttonItr)->getVisibility())
			buttonList.push_back((*buttonItr));
	}

	if (buttonList.size() > 1)
	{
		if (cursorButton)
			buttonList.push_back(cursorButton);

		//start quads from scratch.
		mQuads->removeAllQuads(); 
		for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
			(*buttonItr)->q = 0;  //make sure we are cleared.

		std::vector<button *> zOrderedButtons;
		std::vector<button *>::iterator lowestZButton;
		while (buttonList.size())
		{	
			lowestZButton = buttonList.begin();
			for ( buttonItr = buttonList.begin(); buttonItr != buttonList.end(); ++buttonItr )
			{
				if ((*buttonItr)->zOrder < (*lowestZButton)->zOrder)
					lowestZButton = buttonItr;
			}
			zOrderedButtons.push_back((*lowestZButton));
			buttonList.erase(lowestZButton);
		}

		//give back quads,  but this time in order
		for ( buttonItr = zOrderedButtons.begin(); buttonItr != zOrderedButtons.end(); ++buttonItr )
		{
			(*buttonItr)->q = mQuads->createQuad();
			(*buttonItr)->resetButton();
		}
	}	
}


bool buttonManager::update(void)
{
	unsigned long currentTime = utils.getTime();
	timeDiff = currentTime - lastFrameTime;
	lastFrameTime = currentTime;

	//update all buttons.
	bool wasUpdated = false; //this is to send out a message in case any buttons are fading so that a managing class can optimize drawing if desired
	for ( buttonItr = buttons.begin(); buttonItr != buttons.end(); ++buttonItr )
	{
		if ((*buttonItr)->update())
			wasUpdated = true;
	}

	mQuads->endEdit();
	DrawUtils::draw(mQuads, BLEND_ALPHA);
	mQuads->beginEdit();

	return wasUpdated;
}



button * button::setRotation( float degrees )
{
	rotation = degrees;
	if (q)
		q->setRotation(rotation);

	return this;
}



