/*
	miniGUI
	a smaller and optimized port of buttonGUI for use with the 2D LFA engine
	To make a simple interactive graphical user interface.

	The intention of this library is to provide the simplicity of betaGUI combined with the ease of use and intuitiveness of Navi Library

	This class contains code from and has heavy influence from:
	buttonGUI 
	Navi Library by Adam J. Simmons 
	BetaGUI class by Robin "Betajaen" Southern

	Brought to you by The Lo-Fi Apocalypse    http://www.lfa.com/
	Copyright (C) 2008 Jorge D. Hernandez (metaldev)

	If you use this class,  I love to see your project!  Post screenshots on the LFA forums!

*/

#pragma once
#ifndef LFA_MINIGUI_H
#define LFA_MINIGUI_H

#include <stdio.h>
#include <string>

#include "QuadSet.h"
#include "DrawUtils.h"



namespace miniGUI {

	enum relativePosition
	{
		LEFT,
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		RIGHT,
		BOTTOM_RIGHT,
		BOTTOM_CENTER,
		BOTTOM_LEFT,
		CENTER
	};

	/**
	* An object that holds position-data for a button. Used by buttonManager::createButton and buttonManager::setButtonPosition.
	*/
	class buttonPosition
	{
		bool usingRelative;
		union {
			struct { relativePosition position; short x; short y; } rel;
			struct { short left; short top; } abs;
		} data;

		friend class button;
		friend class buttonManager;
		friend class buttonMesh;
		buttonPosition();
	public:
		/**
		* Creates a relatively-positioned buttonPosition object.
		*
		* @param	relPosition		The position of the button in relation to the Render Window or parent button (if this button is a child).
		*
		* @param	offsetLeft	How many pixels from the left to offset the button from the relative position.
		*
		* @param	offsetTop	How many pixels from the top to offset the button from the relative position.
		*/
		buttonPosition(const relativePosition &relPosition, short offsetLeft = 0, short offsetTop = 0);

		/**
		* Creates an absolutely-positioned buttonPosition object.
		*
		* @param	absoluteLeft	The number of pixels from the left of the Render Window.
		*
		* @param	absoluteTop	The number of pixels from the top of the Render Window.
		*/
		buttonPosition(short absoluteLeft, short absoluteTop);
	};



	enum buttonAction
	{
			ONCLICK = 0,
			ONRELEASE,
			MOUSEOVER,
			MOUSEOFF,
			MOUSEWHEELUP,
			MOUSEWHEELDOWN
	};

	class button;
	class buttonEvent  
	{
	public:
		buttonEvent(buttonAction a, int mouseButtonID, button * mainButton, button * dragDroppedButton = NULL, std::string data = "") :
			action(a),
			mouseButtonID(mouseButtonID),
			actionButton(mainButton),
			droppedButton(dragDroppedButton),
			additionalData(data)
			{}
		buttonAction action;
		int mouseButtonID;
		button * actionButton;
		button * droppedButton;
		std::string additionalData;
	};



	class buttonScheme
	{
	public:
		buttonScheme(float u, float v, float w, float h);
		buttonScheme(LFA::FloatCoords coords);
		buttonScheme(); //makes a blank button
		~buttonScheme();

		//these are pixel texture coords
		LFA::FloatCoords onClick_coords;
		LFA::FloatCoords onRelease_coords; //NOTE:  generally speaking,  for best results  onRelease settings should match mouseOver settings
		LFA::FloatCoords mouseOver_coords;
		LFA::FloatCoords mouseOff_coords;
		
		LFA::Color onClick_color;
		LFA::Color onRelease_color;
		LFA::Color mouseOver_color;
		LFA::Color mouseOff_color;

		bool BLANK;
	};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    **  BUTTON  CLASS    **   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class textInputArea;
	class buttonManager;

	class button 
	{
		friend class buttonManager;

	public:

		/** do not create a button using this.  use the buttonMgr*/
		button(std::string &buttonName, const buttonPosition &position, buttonManager * mgr, button * parentButton, const buttonScheme &style, bool isActiveButton);
			~button();

			/**
			*		Gets the name of this button
			*/
			std::string * getName(void) {return &name;}

			/**
			*		Allows you to choose a new name for this button.
			*/
			button * setName(std::string newName) {name = newName; return this;}

			/**
			*  Adds a sub-button to your button.
			*
			* @param	name		name of the button for later access
			*
			* @param	material		This material name will be used for all states unless:
			*		materialName.onClick  materialName.onRelease   materialName.mouseOver   materialName.mouseOff    
			*		are defined.  You can also use "" or "BLANK"  to have the button be transparent.
			*
			* @param	position		The buttonPosition to set the button to, will define where this button exists on the screen.
			*		
			* @param	width			width of the button,  in pixels.	
			*
			* @param	height		height of the button,  in pixels.	
			*		
			* @param	scheme		the textScheme that you want all text areas associated with this button to have,  if you don't care about it,  use the other override function
			*								and it will inherit the textScheme from the parent button	
			*		
			* @param	isActiveButton	sets whether this button is a static image.  non-Active buttons can't be triggered and can't be moved unless the parent is moved or unless you 
			*										change it later using setStatic();
			*.returns a pointer to the new child button.
			*/
			button * createChildButton(std::string name, relativePosition position, int x, int y, const buttonScheme &scheme, bool isActiveButton = true); //copies width/height from mouseOff_coords in scheme
			button * createChildButton(std::string name, relativePosition position, int x, int y, short  width, short  height, bool isActiveButton = true); //use parent's scheme
			button * createChildButton(std::string name, relativePosition position, int x, int y,short  width, short  height, const buttonScheme &scheme, bool isActiveButton = true);
			


			/**
			* Returns a pointer to the named child button 
			*/
			button * getChildButton(std::string name);

			/**
			* hides the button if it was showing.
			* @param fade   if you want the button to hide smoothly or immediately
			*
			* @param fadeDurationMS   milliseconds for the fade to occur.
			*
			*returns a pointer to self
			*/
			button *  hide(bool fade, unsigned short fadeDurationMS = 300);
			
			/**
			* shows the button if it was hidden
			* @param fade   if you want the button to show smoothly or immediately
			*
			* @param fadeDurationMS   milliseconds for the fade to occur.
			*
			*returns a pointer to self
			*
			*/
			button * show(bool fade, unsigned short fadeDurationMS = 300);

			/**
			*returns if the button is visible or not.
			*/
			bool getVisibility(void); 


			/**
			* sets the look of the button by specifying what UVs it should use during each state
			*/
			button * setButtonScheme( buttonScheme & scheme);

			/**
			* Changes the overall opacity  to a certain percentage.
			* @param	opacity		The opacity percentage as a float. 
			*						Fully Opaque = 1.0, Fully Transparent = 0.0.

			* returns a pointer to self
			*/
			button *  setOpacity(float opacity);

			/** 
			* Sets the vertex color of a button.
			* this gets overridden by events if the button is active.
			*@param	cascadeToScheme    This assigns the given color to all the schemes of this button, thereby making the color 'stick', but losing previous scheme colors.	
			*/
			button *  setColor(const LFA::Color &clr, bool cascadeToScheme = false); 
			button *  setColor(const LFA::Color &upperLeft, const LFA::Color &uppeRight, const LFA::Color &lowerLeft, const LFA::Color &lowerRight); 


			/** 
			* Sets the rotation of the button image.
			* NOTE: this is purely visual and does not affect the button's behavior.
			*/
			button *  setRotation(float degrees);  
			float getRotation(void) {return rotation;}

			/** 
			* Sets the default position of this button to a new position and then moves
			* the button to that position.
			*
			* @param	position		The new buttonPosition to set the button to.

			* returns a pointer to self.
			*/
			button *  setPosition(buttonPosition &position);
			button *  setPosition(int posX, int posY);  //for absolute positioning based on upper left corner of the screen or parent.  

			/** 
			* this will offset a button from its current position, without disturbing its relative positioning 
			* to match a particular coords relative to the screen.
			*/
			button * offsetPositionToScreenCoord(int posX, int posY); 

			/**
			* Moves the button to the last stored position
			*/
			void  resetPosition(void);

			/**
			* Gets whatever position was last given to the button 
			*/
			const buttonPosition * getPosition(void);

			/**
			* Gets the absolute position of the button ( relative to the screen.)
			*/
			buttonPosition getAbsolutePosition(void);
			button * getAbsolutePosition(float pos[2]);

			/**
			*	Sets a new size for the button.
			*	returns a pointer to self
			*/
			button *  setSize(short unsigned int width, short unsigned int height);  

			/**
			*	Accessor function to get the width and height of a button, respectively.
			*/
			void getSize(float dims[2]) {dims[0] = pixelScreenCoords.width; dims[1] = pixelScreenCoords.height;}


			/**
			* Toggles whether or not this button is movable. 
			*
			* @param	m	Whether or not this button should be movable.
			*
			* @param centerButtonOnCursor  Whether the button centers to the mouse when it is dragged,  or whether it is dragged relative to where the onClick occurred (recommended for large buttons).
			*
			* returns a pointer to self.
			*/
			button *  setMovable(bool m , bool centerButtonOnCursor = true)		{movable = m; centerOnGrab = centerButtonOnCursor; return this;}

			/**
			* Returns if the button is allowed to be moved (grabbed) by the user
			*/
			bool getMovable(void)			{return movable;}

			/**
			* Returns if the button is allowed to receive inputs/be moved etc
			*/
			bool getIsActive(void) {if (!state) return false; if (suppressed) return false; return isActive;}

			/**
			* Sets this button to be triggerable in any way.  (set to false if you just want to use it as a background image for example)
			*
			* returns a pointer to self.
			*/
			button *  setActive(bool s) {isActive = s; return this;}

			/**
			* Allows you to set the trigger for a particular action on this button so that it will be reported (or not)
			* inside buttonManager->getEvent()    The only reason to turn of triggers is for efficiency or to reduce feedback from the buttonMgr
			*  
			*
			* @param	action  The buttonAction that you want to set or unset the trigger for.
			*
			* @param onOff		Whether you want to turn on or turn off the trigger
			*
			* returns a pointer to self.
			*/
			button * setTrigger(buttonAction action, bool onOff);

			/**
			* Get the current trigger setting for a particular action on this button.
			*  The trigger determines if this particular buttonAction should be sent into the buttonManager event log.
			*
			* @param	action  The buttonAction that you want to get the trigger for on this button.
			*
			*/
			bool getTrigger(buttonAction action);

			/**
			*  Sets a new parent for this button.  
			*
			* @param  parentName	the name of an existing button to use as a parent.
			* 
			*/
			void setParent(std::string newParentName);
			void setParent(button * newParentButton);

			/**
			* Returns the parent button of this button,  returns NULL if it is a top level button.
			*/
			button * getParent (void) {return parent;}

			/**
			*  Sets the zOrder for this button.
			*  Higher zOrders draw on top of lower zOrders.
			*/
			button * setZOrder(short unsigned int z);

			/**
			*  Sets limits for the movement of this button.
			*  Limits are relative to whatever position type you are using.  
			*  Limits are useful if you want to make a slider or something that can be grabbed but is 'contained'
			*  Simply:  if (movePositionX < x1Limit) movePositionX = x1Limit;
			*
			*  param @useLimits  turn limit checking on/off
			*/
			button * setLimits(bool useLimits);
			button * setLimits(short int x1, short int y1, short int x2, short int y2, bool useLimits = true);

			/**
			*  Returns the current position of the button relative to its lower limit.  
			*  Useful for tracking slider movement.
			*/
			button * getLimitOffset(int offset[2]);


		protected:
				std::string name;
				buttonPosition position; 
				float rotation;  //visual rotation of the button;
				LFA::FloatCoords pixelScreenCoords;  //the last coords given to this button.

				bool movable;
				bool isActive;
				button * parent;	//this will be NULL if the button is a top level button,  otherwise will contain a pointer to the parent button
				LFA::Quad * q;  //the quad pertaining to this button,  if the button is 'BLANK'  will be NULL
				buttonManager * const buttonMgr;
				bool hitTest(int x, int y);
				bool isMouseOver;
				int zOrder;
				std::vector<button*>::iterator buttonItr;
				std::vector<button*> childButtons; 

				buttonScheme style;  //do not allow direct access to this since this is fudged on input to make it intuitive to use: see makeSchemeIntuitive()

				LFA::multiplatform utils;

				bool update(void); //this is protected because you should update all buttons by only calling update() on the manager.
				void cascadeHide(void); //makes button and its children invisible (by removing their quad) without affecting their state.
				void cascadeShow(void); // makes button and its children visible by adding a quad (if needed) without affecting state.
				void setSuppressed(bool onOff);
				bool suppressed;	//suppressed refers to if the button or one if its parents is faded and should not return events
				bool state; //state differs from suppressed in that a button can be suppressed by its parent, but  state can't be set to true/false by the parent.  Therefore state indicates whether this button was turned off/on directly.
				bool fadingOut;
				unsigned long fadingOutStart;
				unsigned long fadingOutEnd;
				bool fadingIn;
				unsigned long fadingInStart;
				unsigned long fadingInEnd;

				bool trigger_onClick;
				bool trigger_onRelease;
				bool trigger_mouseOver;
				bool trigger_mouseOff;

				bool onClick(void);  //argument states if the onClick event should be passed down to the buttonMeshes
				bool onRelease(void);
				bool mouseOver(void);
				bool mouseOff(void);

				bool centerOnGrab;	//whether button centers to mouse when grabbed.

				void resetButton(bool adjustScheme = false); //Resets style, position, and size.  Useful in the code during hiding/showing buttons and reordering buttons to accommodate z values.

				bool useLimits;
				short int x1Limit; // these are for using this button as a slider or something that you want to limit how far it can be moved from the parent.  0 = no limit
				short int x2Limit;
				short int y1Limit;
				short int y2Limit;

				//these two methods are a bit of a workaround to keep the interface clean and counter for an unintuitive UV scheme in quads
				button * setButtonScheme( buttonScheme & scheme, bool adjust); //this is private since only internally can we 'not' adjust it - hence making adjustment invisible to the interface.
				void makeSchemeIntuitive(buttonScheme & scheme);  //this is a fudge factor algorithm to reverse the Y calculation in the UV of the scheme to make them intuitive to use through the interface.
	};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    **  BUTTON  MANAGER CLASS    **   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class buttonManager
	{
		friend class button;
	public:

		buttonManager(std::string buttonImagesTextureFile, buttonScheme defaultButtonScheme, unsigned int screenWidth, unsigned int screenHeight);
		~buttonManager(void);


		/**
		* You can customize the mouse cursor by using this function.
		*  NOTE:  a  button instance is created to represent the cursor graphic.  You can use any of the normal functionality of a button
		*  on the cursor with the exception of setParent() which will have no effect.
		*  returns a pointer to the special button assigned to the cursor, see  getCursorButton()  for more info.
		*
		* The cursor will use the default button texture.
		*
		* @param	texCoords	The UV pixels of the cursor.  it will map 1:1 to pixels on the screen.
		*
		* @param	hotspotX/hotspotY	An offset to use as a trigger point for the cursor.  If your graphic does not lend itself well to pointing from its upper left corner, for example.
		*
		* @param	visibility		Should the cursor be hidden?
		*
		*/
		button * setCursor(LFA::FloatCoords &texCoords,  unsigned short hotspotX,  unsigned short hotspotY, bool visibility);

		/**
		* Displays the cursor.
		*	NOTE: won't do anything if you have not called setCursor()
		*	returns a pointer to the cursorButton;
		*/
		button * showCursor(void);
		/**
		* Hides the cursor.
		*	NOTE: won't do anything if you have not called setCursor()
		*	returns a pointer to the cursorButton;
		*/
		button * hideCursor(void);

		/**
		* Injects the mouse's current coordinates in absolute coords when it moves
		* NOTE:  unlike click events, buttons do not block each other from mouseOver and mouseOff commands 
		*/
		bool injectMouseMove(int xPos, int yPos);

		/**
		* Inject a mouse down at the current position of the mouse, you can specify the pressed button.
		*  NOTE:  only the topmost button will be affected
		*/
		bool injectMouseDown(int mouseButttonIndex = 0);

		/**
		* Inject a mouse up at the current position of the mouse, you can specify the raised button.
		*/
		bool injectMouseUp(int mouseButttonIndex = 0);

		/**
		* Inject rotative clicks of the mousewheel as a relative int.   Ex.   1 = one click up,  -2 = two clicks down 
		*/
		bool injectMouseWheel(short int z);

		

		/**
		* Creates a button.
		*
		* @param	name	The name of the button, used to refer to a specific button in subsequent calls.
		*
		* @param material  The name of the material you want to be applied to the button.  You can use "BLANK" to leave it without a material.
		*							A given material name will recognize variants such as  materialName.onClick  materialName.onRelease   materialName.mouseOver   materialName.mouseOff
		*							and use them as appropriate
		*
		* @param	buttonPosition	The unified position (either relative or absolute) of a button.
		*
		* @param	width		The width of the button.
		*
		* @param	height	The height of the button.
		*
		* @param	zOrder		Sets the starting Z-Order for this button; buttons with higher Z-Orders will be on top of other
		*						buttons. To auto-increment this value for every successive button, leave this parameter as '0'.
		*
		*/
		button* createButton (std::string name,  const buttonPosition &position, const buttonScheme &scheme, unsigned short zOrder = 0,  bool isActive = true);  //uses width/height from mouseOff_coords
		button* createButton (std::string name,  const buttonPosition &position, unsigned short width, unsigned short height, unsigned short zOrder = 0,  bool isActive = true);
		button* createButton (std::string name,  const buttonPosition &position, unsigned short width, unsigned short height, const buttonScheme &scheme, unsigned short zOrder = 0, bool isActive = true);

		/*
		* set new defaults for future buttons.
		*/
		void setDefaults( buttonScheme &scheme);

		/*
		* Call this when your resolution changes so that buttonMeshes can be adjusted to the new settings.
		*/
		void resetScreenResolution(unsigned int x, unsigned int y);  

		/**
		* Returns a pointer to the button with the given name,  NULL if it doesnt exist.
		*
		*  @param		buttonName	The name of the button you created earlier that you now want to get a pointer for.
		*/
		button * getButton(std::string buttonName);

		/**
		* Deletes a particular button.
		*
		*  @param		name	The name of the button or textInputArea you want to destroy.
		*  NOTE:  this function is a little dangerous,  make sure you aren't holding any pointers to deleted buttons or children of deleted buttons.
		*	also make sure you are checking for NULL in places you are asking for getButton("name") in case "name"  has been deleted now.
		*	If in doubt,  just hide buttons until your program is done and call shutdown()
		*/
		void deleteButton(std::string buttonName);
		void deleteButton(button * b);

		/**
		*  clears the buttonMgr of all buttons
		*/
		void deleteAllButtons(void);


		/**
		* Forces the mouse to a position relative to the top left of the given button
		*
		*  @param		buttonName	The button whose position you want to click relative to.
		*  @param		x					Horizontal offset from upper left of the given button.
		*  @param		y					Vertical offset from upper left of the given button.
		*  returns true on success,  false if the button was not found.
		*/
		bool forceMove(std::string &buttonName, short x, short y);

		/**
		* Forces a mouse down and a mouse up wherever the mouse is.
		*
		*  @param		mouseButton	The mouse button you want to force a click with.
		*/
		void forceClick(int mouseButttonIndex = 0);

		/**
		* Forces a mouse to click down and then mouse up over a button,  then returns the mouse to its original location
		* NOTE: button must be active for it to work
		*/
		void forceClickHidden(std::string buttonName);


		/**
		* Returns the last event that occurred since the last time this was called.
		* You should continue calling this function until it returns NULL to clear out all events.
		*/
		buttonEvent * getEvent(void);

		/**
		* Returns a pointer to the cursor button.
		* NOTE: the cursor button is not a regular button,  it does not report nor receive hit events.  You cannot retrieve it using getButton("name").
		* You may however wish to use this pointer for the setParent() of another button if you wish to parent buttons to the cursor.  If you choose to do this,
		* beware of your button's relative position as you may block the hotspot and keep the cursor from triggering onClick events of other, unparented buttons.
		*/
		button * getCursorButton(void) {return cursorButton;}

		/**
		* Returns the currently grabbed button, if any.  Otherwise null. 
		*/
		button * getGrabbedButton(void) {return grabbedButton;}

		/**
		* force a button to be grabbed.  Returns if it was successful or not.
		*/
		bool injectGrabbedButton(button * b);

		/**
		* Returns the current hotspot position of the cursor 
		*/
		buttonPosition getCursorPosition(void) {return buttonPosition(mouseX, mouseY);}
		void getCursorPosition(float pos[2]) {	pos[0] = mouseX; pos[1] = mouseY; }

		/**
		* Call this every frame to have your buttons behave properly.
		* If there were any fading updates done this frame it will return true.
		*/
		bool update(void);

		/**
		* Calls delete on all buttons and destroys all overlays to prepare for the instance of the manager to be deleted and set to NULL
		*/
		void shutdown(void);
	
		/**
		* Returns the raw vector containing pointers to all buttons (except cursor button.)
		*/
		std::vector<button*> getAllButtons(void) {return buttons;}

	private:

		LFA::QuadSet * const mQuads;
		void reorderQuads(void); //reorders button drawing according to their z values

		buttonScheme defaultButtonScheme;
		
		button * getTopButton(void);

		short int mouseX;
		short int mouseY;
		short int mouseOffsetX;
		short int mouseOffsetY;

		std::vector<button*>::iterator buttonItr;
		std::vector<button*> buttons; //contains pointers to all the buttons created through the manager 

		unsigned long lastFrameTime;
		unsigned long timeDiff;

		button * grabbedButton;
		button * cursorButton;  //can be used as an attach point for other buttons, will never report events.

		LFA::multiplatform utils;

		static const short triggerOpacity = 50;   //any buttons  will not react when their opacity is less than this percentage

		unsigned int screenResX;
		unsigned int screenResY;

		unsigned short zOrderCounter;
		int grabbingMouseButton;

		std::string eventStringContainer;  //temporary string for transferring event data.
		buttonEvent eventContainer;
		std::vector<buttonEvent> eventLog;
	};
}
#endif //LFA_MINIGUI_H