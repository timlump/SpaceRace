// SpaceRace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "World.h"
#include "dirent.h"

#pragma region GLOBALS
//GLOBALS
GLFWmonitor *monitor = NULL;
GLFWwindow *window = NULL;
int width,height;

//modules
lua_State *luaState = NULL;
TwBar *bar = NULL;
World *world = NULL;

SDL_Joystick* controller = NULL;

CEGUI::Window *mainMenu = NULL;
CEGUI::Window *joinMenu = NULL;
CEGUI::Window *hostMenu = NULL;
CEGUI::Window *settingsMenu = NULL;
CEGUI::Window *gameMenu = NULL;
CEGUI::Window *currentMenu = NULL;

std::map<int,KeyType> keyBindings;
std::map<int,KeyType> mouseBindings;
std::map<int,KeyType> joystickBindings;

std::map<KeyType,int> tempKeyBindings;
std::map<KeyType,int> tempMouseBindings;
std::map<KeyType,int> tempJoystickBindings;

enum BindType{KEY_BIND,MOUSE_BIND,JOY_BIND};
#pragma endregion GLOBALS

//functions
void initialiseEngine();
void destroyEngine();

void switchMenu(CEGUI::Window *window)
{
	currentMenu->hide();
	currentMenu = window;
	currentMenu->show();
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(currentMenu);
}

void setPreviewImage(CEGUI::String filename)
{
	try
	{
		CEGUI::DefaultWindow *previewImage = (CEGUI::DefaultWindow*)currentMenu->getChild("PreviewImage");
		try
		{
			CEGUI::ImageManager::getSingleton().addFromImageFile(filename+"_preview","MapPreviews/"+filename+".png");
		}
		catch(CEGUI::AlreadyExistsException e)
		{
			//ignore
		}
		previewImage->setProperty("Image",filename+"_preview");
	}
	catch (CEGUI::Exception e)
	{
		printf("CEGUI Exception: %s\n",e.what());
	}
}

//http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
void populateMapList()
{
	//lets clear out the combobox
	CEGUI::Combobox *mapList = (CEGUI::Combobox *)currentMenu->getChild("MapComboBox");
	mapList->resetList();
	DIR *dir = opendir(MAP_PATH);
	bool success = false;
	struct dirent *ent;
	if(dir)
	{
		while((ent = readdir(dir))!=NULL)
		{
			std::string name(ent->d_name);
			//lets see if it is the correct format for a map
			if(name.find(MAP_FORMAT)!=std::string::npos)
			{
				CEGUI::ListboxItem *item = new CEGUI::ListboxTextItem(name);
				item->setAutoDeleted(true);
				mapList->addItem(item);
				success = true;
			}
		}
		closedir(dir);
	}

	if(success)
	{
		CEGUI::ListboxItem *item = mapList->getListboxItemFromIndex(0);
		setPreviewImage(item->getText());
		mapList->setItemSelectState(item,true);
	}
}

#pragma region KEY_BINDING_CODE
//http://cegui.org.uk/wiki/GLFW_to_CEGUI_Key_and_Mouse_Mapping
CEGUI::Key::Scan GlfwToCeguiKey(int glfwKey)
{
	switch(glfwKey)
	{
		case GLFW_KEY_UNKNOWN	: return CEGUI::Key::Unknown;
		case GLFW_KEY_ESCAPE	: return CEGUI::Key::Escape;
		case GLFW_KEY_F1	: return CEGUI::Key::F1;
		case GLFW_KEY_F2	: return CEGUI::Key::F2;
		case GLFW_KEY_F3	: return CEGUI::Key::F3;
		case GLFW_KEY_F4	: return CEGUI::Key::F4;
		case GLFW_KEY_F5	: return CEGUI::Key::F5;
		case GLFW_KEY_F6	: return CEGUI::Key::F6;
		case GLFW_KEY_F7	: return CEGUI::Key::F7;
		case GLFW_KEY_F8	: return CEGUI::Key::F8;
		case GLFW_KEY_F9	: return CEGUI::Key::F9;
		case GLFW_KEY_F10       : return CEGUI::Key::F10;
		case GLFW_KEY_F11       : return CEGUI::Key::F11;
		case GLFW_KEY_F12       : return CEGUI::Key::F12;
		case GLFW_KEY_F13       : return CEGUI::Key::F13;
		case GLFW_KEY_F14       : return CEGUI::Key::F14;
		case GLFW_KEY_F15       : return CEGUI::Key::F15;
		case GLFW_KEY_UP        : return CEGUI::Key::ArrowUp;
		case GLFW_KEY_DOWN      : return CEGUI::Key::ArrowDown;
		case GLFW_KEY_LEFT      : return CEGUI::Key::ArrowLeft;
		case GLFW_KEY_RIGHT     : return CEGUI::Key::ArrowRight;
		case GLFW_KEY_LEFT_SHIFT    : return CEGUI::Key::LeftShift;
		case GLFW_KEY_RIGHT_SHIFT    : return CEGUI::Key::RightShift;
		case GLFW_KEY_LEFT_CONTROL     : return CEGUI::Key::LeftControl;
		case GLFW_KEY_RIGHT_CONTROL    : return CEGUI::Key::RightControl;
		case GLFW_KEY_LEFT_ALT      : return CEGUI::Key::LeftAlt;
		case GLFW_KEY_RIGHT_ALT      : return CEGUI::Key::RightAlt;
		case GLFW_KEY_TAB       : return CEGUI::Key::Tab;
		case GLFW_KEY_ENTER     : return CEGUI::Key::Return;
		case GLFW_KEY_BACKSPACE : return CEGUI::Key::Backspace;
		case GLFW_KEY_INSERT    : return CEGUI::Key::Insert;
		case GLFW_KEY_DELETE       : return CEGUI::Key::Delete;
		case GLFW_KEY_PAGE_UP    : return CEGUI::Key::PageUp;
		case GLFW_KEY_PAGE_DOWN  : return CEGUI::Key::PageDown;
		case GLFW_KEY_HOME      : return CEGUI::Key::Home;
		case GLFW_KEY_END       : return CEGUI::Key::End;
		case GLFW_KEY_KP_ENTER	: return CEGUI::Key::NumpadEnter;
		default			: return CEGUI::Key::Unknown;
	}
}
 
CEGUI::MouseButton GlfwToCeguiButton(int glfwButton)
{
	switch(glfwButton)
	{
		case GLFW_MOUSE_BUTTON_LEFT	: return CEGUI::LeftButton;
		case GLFW_MOUSE_BUTTON_RIGHT	: return CEGUI::RightButton;
		case GLFW_MOUSE_BUTTON_MIDDLE	: return CEGUI::MiddleButton;
		default				: return CEGUI::NoButton;
	}
}

std::string glfwKeyToString(int glfwKey)
{
	std::string result = "";

	if(glfwKey > 32 && glfwKey <= 96)
	{
		result += (char)glfwKey;
	}
	else if(glfwKey >= 290 && glfwKey <= 301)
	{
		char buffer[50];
		sprintf(buffer,"F%d",glfwKey-289);
		result = std::string(buffer);
	}
	else
	{
		switch(glfwKey)
		{
		case 32:
			result = "Space";
			break;
		case 257:
			result = "Enter";
			break;
		case 259:
			result = "Delete";
			break;
		case 260:
			result = "Insert";
			break;
		case 261:
			result = "Delete";
			break;
		case 266:
			result = "PgUp";
			break;
		case 267:
			result = "PgDn";
			break;
		case 268:
			result = "Home";
			break;
		case 269:
			result = "End";
			break;
		case 340:
			result = "LShift";
			break;
		case 341:
			result = "LCtrl";
			break;
		case 342:
			result = "LAlt";
			break;
		case 346:
			result = "RAlt";
			break;
		case 344:
			result = "RShift";
			break;
		case 345:
			result = "RCtrl";
			break;
		}
	}

	return result;
}

std::string actionKeyToKeyBox(KeyType type)
{
	switch(type)
	{
	case KeyType::FORWARD:
		return "ForwardBox";
	case KeyType::BACKWARD:
		return "BackwardBox";
	case KeyType::LEFT:
		return "LeftBox";
	case KeyType::RIGHT:
		return "RightBox";
	case KeyType::LEAN_LEFT:
		return "LeanLeftBox";
	case KeyType::LEAN_RIGHT:
		return "LeanRightBox";
	case KeyType::JUMP:
		return "JumpBox";
	case KeyType::CROUCH:
		return "CrouchBox";
	case KeyType::PRONE:
		return "ProneBox";
	case KeyType::SPIN_180:
		return "SpinBox";
	case KeyType::FIRE:
		return "FireBox";
	case KeyType::MELEE:
		return "MeleeBox";
	case KeyType::RELOAD:
		return "ReloadBox";
	case KeyType::AIM:
		return "AimBox";
	case KeyType::FLASHLIGHT:
		return "FlashLightBox";
	case KeyType::USE:
		return "UseBox";
	case KeyType::FREELOOK:
		return "FreelookBox";
	case KeyType::TALK:
		return "TalkBox";
	case KeyType::SLOT_0:
		return "Slot0Box";
	case KeyType::SLOT_1:
		return "Slot1Box";
	case KeyType::SLOT_2:
		return "Slot2Box";
	case KeyType::SLOT_3:
		return "Slot3Box";
	case KeyType::SLOT_4:
		return "Slot4Box";
	case KeyType::SLOT_5:
		return "Slot5Box";
	case KeyType::SLOT_6:
		return "Slot6Box";
	case KeyType::SLOT_7:
		return "Slot7Box";
	case KeyType::SLOT_8:
		return "Slot8Box";
	case KeyType::SLOT_UP:
		return "SlotUpBox";
	case KeyType::SLOW_DOWN:
		return "SlotDownBox";
	case KeyType::CONFIRM:
		return "ConfirmBox";
	default:
		return "Error";
	}
}

void tempBind(KeyType target,int source,BindType type)
{
	if(type == BindType::KEY_BIND)
	{
		tempKeyBindings[target] = source;
		//delete any duplicate binds
		auto it = tempJoystickBindings.find(target);
		if(it!=tempJoystickBindings.end())
		{
			tempJoystickBindings.erase(it);
		}

		it = tempMouseBindings.find(target);
		if(it!=tempMouseBindings.end())
		{
			tempMouseBindings.erase(it);
		}
	}
	else if(type == BindType::JOY_BIND)
	{
		tempJoystickBindings[target] = source;
		//delete any duplicate binds
		auto it = tempMouseBindings.find(target);
		if(it!=tempMouseBindings.end())
		{
			tempMouseBindings.erase(it);
		}

		it = tempKeyBindings.find(target);
		if(it!=tempKeyBindings.end())
		{
			tempKeyBindings.erase(it);
		}
	}
	else
	{
		tempMouseBindings[target] = source;
		//delete any duplicate binds
		auto it = tempJoystickBindings.find(target);
		if(it!=tempJoystickBindings.end())
		{
			tempJoystickBindings.erase(it);
		}

		it = tempKeyBindings.find(target);
		if(it!=tempKeyBindings.end())
		{
			tempKeyBindings.erase(it);
		}
	}
}

void binding(std::string name,KeyType target, int source, BindType type)
{
	CEGUI::Editbox *editBox = (CEGUI::Editbox *)currentMenu->getChild(name);
	if(editBox && editBox->isActive())
	{
		char buffer[50];
		std::string result;
		bool assign = false;
		switch(type)
		{
		case BindType::MOUSE_BIND:
			sprintf(buffer,"Mouse_%d",source);
			assign = true;
			tempBind(target,source,type);
			break;
		case BindType::KEY_BIND:
			result = glfwKeyToString(source);
			if(!result.empty())
			{
				sprintf(buffer,"%s",glfwKeyToString(source).c_str());
				assign = true;
				tempBind(target,source,type);
			}
			break;
		case BindType::JOY_BIND:
			sprintf(buffer,"Joy_%d",source);
			assign = true;
			tempBind(target,source,type);
			break;
		}
		if(assign)
		{
			editBox->setText(buffer);
		}
	}
}

void bindKeys(int key, BindType type)
{
	binding("ForwardBox",KeyType::FORWARD,key,type);
	binding("BackwardBox",KeyType::BACKWARD,key,type);
	binding("LeftBox",KeyType::LEFT,key,type);
	binding("RightBox",KeyType::RIGHT,key,type);
	binding("LeanLeftBox",KeyType::LEAN_LEFT,key,type);
	binding("LeanRightBox",KeyType::LEAN_RIGHT,key,type);
	binding("JumpBox",KeyType::JUMP,key,type);
	binding("CrouchBox",KeyType::CROUCH,key,type);
	binding("ProneBox",KeyType::PRONE,key,type);
	binding("SpinBox",KeyType::SPIN_180,key,type);
	binding("FireBox",KeyType::FIRE,key,type);
	binding("MeleeBox",KeyType::MELEE,key,type);
	binding("ReloadBox",KeyType::RELOAD,key,type);
	binding("AimBox",KeyType::AIM,key,type);
	binding("FlashLightBox",KeyType::FLASHLIGHT,key,type);
	binding("UseBox",KeyType::USE,key,type);
	binding("FreelookBox",KeyType::FREELOOK,key,type);
	binding("TalkBox",KeyType::TALK,key,type);
	binding("Slot0Box",KeyType::SLOT_0,key,type);
	binding("Slot1Box",KeyType::SLOT_1,key,type);
	binding("Slot2Box",KeyType::SLOT_2,key,type);
	binding("Slot3Box",KeyType::SLOT_3,key,type);
	binding("Slot4Box",KeyType::SLOT_4,key,type);
	binding("Slot5Box",KeyType::SLOT_5,key,type);
	binding("Slot6Box",KeyType::SLOT_6,key,type);
	binding("Slot7Box",KeyType::SLOT_7,key,type);
	binding("Slot8Box",KeyType::SLOT_8,key,type);
	binding("SlotUpBox",KeyType::SLOT_UP,key,type);
	binding("SlotDownBox",KeyType::SLOW_DOWN,key,type);
	binding("ConfirmBox",KeyType::CONFIRM,key,type);
}

void loadKeyBinding(std::string filename)
{
	tempKeyBindings.clear();
	tempMouseBindings.clear();
	tempJoystickBindings.clear();

	keyBindings.clear();
	mouseBindings.clear();
	joystickBindings.clear();

	std::ifstream file(filename);
	//http://stackoverflow.com/questions/7868936/read-file-line-by-line
	std::string line;
	BindType type = BindType::KEY_BIND;
	while(std::getline(file,line))
	{
		//determine type of binding
		if(!line.compare("K"))
		{
			type = BindType::KEY_BIND;
		}
		else if(!line.compare("J"))
		{
			type = BindType::JOY_BIND;
		}
		else if(!line.compare("M"))
		{
			type = BindType::MOUSE_BIND;
		}
		//parse
		else
		{
			std::istringstream iss(line);
			int source,target;
			iss >> source >> target;
			switch(type)
			{
			case BindType::KEY_BIND:
				tempKeyBindings[(KeyType)target] = source;
				keyBindings[source] = (KeyType)target;
				break;
			case BindType::JOY_BIND:
				tempJoystickBindings[(KeyType)target] = source;
				joystickBindings[source] = (KeyType)target;
				break;
			case BindType::MOUSE_BIND:
				tempMouseBindings[(KeyType)target] = source;
				mouseBindings[source] = (KeyType)target;
				break;
			}
		}
		
	}
}

void saveKeyBinding(std::string filename)
{
	keyBindings.clear();
	mouseBindings.clear();
	joystickBindings.clear();

	std::ofstream file(filename);
	std::map<KeyType,int>::iterator it;
	file << "K\n";
	for(it = tempKeyBindings.begin() ; it!=tempKeyBindings.end() ; it++)
	{
		file << it->second << " " << it->first << std::endl;
		keyBindings[it->second] = it->first;
	}
	file << "J\n";
	for(it = tempJoystickBindings.begin() ; it!=tempJoystickBindings.end() ; it++)
	{
		file << it->second << " " << it->first << std::endl;
		joystickBindings[it->second] = it->first;
	}
	file << "M\n";
	for(it = tempMouseBindings.begin() ; it!=tempMouseBindings.end() ; it++)
	{
		file << it->second << " " << it->first << std::endl;
		mouseBindings[it->second] = it->first;
	}
	file.close();
}
#pragma endregion KEY_BINDING_CODE

#pragma region CALLBACKS
//GLFW callbacks
void charCallback(GLFWwindow* window, unsigned int unicode)
{
	if(CEGUI::System::getSingletonPtr() && currentMenu != settingsMenu)
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(unicode);
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch(key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window,GL_TRUE);
			break;
		}

		if(CEGUI::System::getSingletonPtr())
		{
			CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(GlfwToCeguiKey(key));
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if(CEGUI::System::getSingletonPtr())
		{
			CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(GlfwToCeguiKey(key));
		}
	}

	if(keyBindings.find(key)!=keyBindings.end())
	{
		world->control(InputType::KEY,keyBindings[key],(action==GLFW_PRESS)?KeyAction::PRESS:KeyAction::RELEASE);
	}

	if(currentMenu == settingsMenu)
	{
		bindKeys(key,BindType::KEY_BIND);
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(CEGUI::System::getSingletonPtr())
	{
		if(action == GLFW_PRESS)
		{
			CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(GlfwToCeguiButton(button));
		}
		else if (action == GLFW_RELEASE)
		{
			CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(GlfwToCeguiButton(button));
		}
	}

	if(mouseBindings.find(button)!=mouseBindings.end())
	{
		world->control(InputType::KEY,mouseBindings[button],(action==GLFW_PRESS)?KeyAction::PRESS:KeyAction::RELEASE);
	}

	if(currentMenu == settingsMenu)
	{
		bindKeys(button,BindType::MOUSE_BIND);
	}
}

void mouseMotionCallback(GLFWwindow* window, double xpos, double ypos)
{
	double currentMouseX, currentMouseY;
	glfwGetCursorPos(window,&currentMouseX,&currentMouseY);
	glfwSetCursorPos(window,(double)width/2,(double)height/2);

	float mouseDX = currentMouseX-(double)width/2;
	float mouseDY = currentMouseY-(double)height/2;

	if(CEGUI::System::getSingletonPtr())
	{
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(mouseDX,mouseDY);
	}

	world->control(InputType::MOUSE_X,mouseDX);
	world->control(InputType::MOUSE_Y,mouseDY);
}

//CEGUI callbacks - http://cegui.org.uk/wiki/Identifying_Multiple_Event_Sources_From_A_Single_Callback
bool ceguiButtonClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(e);

	CEGUI::String senderID = we.window->getName();

	if(senderID == "QuitButton")
	{
		glfwSetWindowShouldClose(window,GL_TRUE);
	}
	else if(senderID == "HostButton")
	{
		switchMenu(hostMenu);
		populateMapList();
	}
	else if(senderID == "SettingsButton")
	{
		switchMenu(settingsMenu);
	}
	else if(senderID == "HostBackButton")
	{
		switchMenu(mainMenu);
	}
	else if(senderID == "HostStartButton")
	{
		//to do
	}
	else if(senderID == "SettingsBackButton")
	{
		switchMenu(mainMenu);
	}
	else if(senderID == "SettingsApplyButton")
	{
		saveKeyBinding(KEYBIND_PATH"binds.ini");
	}

	return true;
}

bool mapSelectionAccepted(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox *mapComboBox = (CEGUI::Combobox *)currentMenu->getChild("MapComboBox");
	CEGUI::ListboxTextItem *item = (CEGUI::ListboxTextItem*)mapComboBox->getSelectedItem();
	setPreviewImage(item->getText());
	return true;
}
#pragma endregion CALLBACKS

//Anton's OpenGL 4 Tutorials - http://antongerdelan.net/opengl/
void updateFrameRate()
{
	static double prevSeconds = glfwGetTime();
	static int frameCount;
	double currentSeconds = glfwGetTime();
	double elapsedSeconds = currentSeconds-prevSeconds;
	if(elapsedSeconds > FPS_TRACK_DELAY)
	{
		prevSeconds = currentSeconds;
		double fps = (double)frameCount/elapsedSeconds;
		char tmp[128];
		sprintf(tmp,"Space Race @ FPS: %.2f",fps);
		glfwSetWindowTitle(window,tmp);
		frameCount = 0;
	}
	frameCount++;
};

double getTimeStep()
{
	static double prevTime = glfwGetTime();
	double currentTime = glfwGetTime();
	double diff = currentTime-prevTime;
	prevTime = currentTime;
	return diff;
}

void logic()
{
	float timeStep = getTimeStep();
	world->update(timeStep);
	if(CEGUI::System::getSingletonPtr())
	{
		CEGUI::System::getSingleton().injectTimePulse(timeStep);
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	world->draw();

	glDisable(GL_DEPTH_TEST);
	if(CEGUI::System::getSingletonPtr())
	{
		CEGUI::System::getSingleton().renderAllGUIContexts();
	}
	glEnable(GL_DEPTH_TEST);
	
	glfwSwapBuffers(window);
}

void io()
{
	glfwPollEvents();
	//handle joystick
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_JOYAXISMOTION:
			if(event.jaxis.value < -DEADZONE || event.jaxis.value > DEADZONE)
			{
				//x axis
				if(event.jaxis.axis == 0)
				{
					world->control(InputType::JOY_X,(float)event.jaxis.value/AXIS_RANGE);
				}
				else if(event.jaxis.axis == 1)
				{
					world->control(InputType::JOY_Y,(float)event.jaxis.value/AXIS_RANGE);
				}
			}
			break;
		case SDL_JOYBUTTONDOWN:
			//printf("Down: %d\n",event.jbutton.button);
			if(currentMenu == settingsMenu)
			{
				bindKeys(event.jbutton.button,BindType::JOY_BIND);
			}
			else
			{
				if(joystickBindings.find(event.jbutton.button)!=joystickBindings.end())
				{
					world->control(InputType::KEY,joystickBindings[event.jbutton.button],KeyAction::PRESS);
				}
			}
			break;
		case SDL_JOYBUTTONUP:
			//printf("Up: %d\n",event.jbutton.button);
			if(joystickBindings.find(event.jbutton.button)!=joystickBindings.end())
			{
				world->control(InputType::KEY,joystickBindings[event.jbutton.button],KeyAction::RELEASE);
			}
			break;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	initialiseEngine();

	//scripting
	luaState = luaL_newstate();
	luaL_openlibs(luaState);
	luabind::open(luaState);

	//bind classes to lua
	World::registerWithLua(luaState);

	//setup script
	/*try
	{
		luabind::call_function<void>(luaState,"dofile",SCRIPT_PATH"setup.lua");
	}
	catch (luabind::error e)
	{
		printf("Exception: %s\n",e.what());
	}*/

#pragma region CORE
	//main loop
	while(!glfwWindowShouldClose(window))
	{
		updateFrameRate();
		logic();
		draw();
		io();
	}
#pragma endregion CORE

	destroyEngine();
	return 0;
}

void initialiseEngine()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES,16);

	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *vmode = glfwGetVideoMode(monitor);

	width = vmode->width;
	height = vmode->height;

	window = glfwCreateWindow(vmode->width,vmode->height,"Space Race",monitor,NULL);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
	glfwSetKeyCallback(window,keyCallback);
	glfwSetCursorPosCallback(window,mouseMotionCallback);
	glfwSetMouseButtonCallback(window,mouseButtonCallback);
	glfwSetCharCallback(window,charCallback);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//devil
	ilInit();

	loadKeyBinding(KEYBIND_PATH"binds.ini");

	//interface
	CEGUI::OpenGL3Renderer::bootstrapSystem();
	//load resources
	CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
	rp->setResourceGroupDirectory("imagesets","../../../Media/UI/datafiles/imagesets/");
	rp->setResourceGroupDirectory("fonts","../../../Media/UI/datafiles/fonts/");
	rp->setResourceGroupDirectory("layouts","../../../Media/UI/datafiles/layouts/");
	rp->setResourceGroupDirectory("looknfeels","../../../Media/UI/datafiles/looknfeel/");
	rp->setResourceGroupDirectory("schemes","../../../Media/UI/datafiles/schemes/");
	rp->setResourceGroupDirectory("lua_scripts","../../../Media/UI/datafiles/lua_scripts/");

	//setup resources
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");

	try
	{
		CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
		CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-12.font");
		CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
		CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultTooltipType("TaharezLook/Tooltip");
		
		mainMenu = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("SpaceRace_MainMenu.layout");
		CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(mainMenu);
		mainMenu->show();
		currentMenu = mainMenu;

		settingsMenu = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("SpaceRace_SettingsMenu.layout");
		settingsMenu->hide();

		hostMenu = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("SpaceRace_HostMenu.layout");
		hostMenu->hide();

		//wire up callbacks
		mainMenu->getChild("SettingsButton")->subscribeEvent(CEGUI::PushButton::EventClicked,&ceguiButtonClick);
		mainMenu->getChild("HostButton")->subscribeEvent(CEGUI::PushButton::EventClicked,&ceguiButtonClick);
		mainMenu->getChild("QuitButton")->subscribeEvent(CEGUI::PushButton::EventClicked,&ceguiButtonClick);

		hostMenu->getChild("HostStartButton")->subscribeEvent(CEGUI::PushButton::EventClicked,&ceguiButtonClick);
		hostMenu->getChild("HostBackButton")->subscribeEvent(CEGUI::PushButton::EventClicked,&ceguiButtonClick);
		hostMenu->getChild("MapComboBox")->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,&mapSelectionAccepted);

		settingsMenu->getChild("SettingsBackButton")->subscribeEvent(CEGUI::PushButton::EventClicked,&ceguiButtonClick);
		settingsMenu->getChild("SettingsApplyButton")->subscribeEvent(CEGUI::PushButton::EventClicked,&ceguiButtonClick);


		//fill up settings menu with keybinds
		std::map<KeyType,int>::iterator it;

		//keyboard
		for(it = tempKeyBindings.begin() ; it != tempKeyBindings.end() ; it++)
		{
			settingsMenu->getChild(actionKeyToKeyBox(it->first))->setText(glfwKeyToString(it->second));
		}

		//mouse
		for(it = tempMouseBindings.begin() ; it != tempMouseBindings.end() ; it++)
		{
			char buffer[50];
			sprintf(buffer,"Mouse_%d",it->second);
			settingsMenu->getChild(actionKeyToKeyBox(it->first))->setText(std::string(buffer));
		}

		//joystick
		for(it = tempJoystickBindings.begin() ; it != tempJoystickBindings.end() ; it++)
		{
			char buffer[50];
			sprintf(buffer,"Joy_%d",it->second);
			settingsMenu->getChild(actionKeyToKeyBox(it->first))->setText(std::string(buffer));
		}
	}
	catch(CEGUI::Exception e)
	{
		printf("CEGUI Exception: %s\n",e.what());
	}

	

	SDL_Init(SDL_INIT_JOYSTICK);
	if(SDL_NumJoysticks() < 1)
	{
		printf("No joysticks!\n");
	}

	else
	{
		SDL_JoystickEventState(SDL_ENABLE);
		controller = SDL_JoystickOpen(0);
		if(controller)
		{
			printf("Using %s\n",SDL_JoystickNameForIndex(0));
		}
	}

	world = new World(luaState);
}

void destroyEngine()
{
	delete world;
	if(controller)
	{
		SDL_JoystickClose(controller);
	}
	SDL_Quit();
	lua_close(luaState);
	glfwTerminate();
}