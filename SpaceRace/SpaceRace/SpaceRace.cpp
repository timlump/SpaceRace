// SpaceRace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "World.h"
#include "dirent.h"
#include "UtilFunctions.h"

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
void setupCEGUI();


#pragma region KEY_BINDING_CODE
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
bool ceguiMouseEventClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(e);

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
			if(joystickBindings.find(event.jbutton.button)!=joystickBindings.end())
			{
				world->control(InputType::KEY,joystickBindings[event.jbutton.button],KeyAction::PRESS);
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
	luabind::globals(luaState)["World"] = world;

	world->setLuaState(luaState,SCRIPT_PATH"worldUpdate.lua");

	//setup script
	try
	{
		luabind::call_function<void>(luaState,"dofile",SCRIPT_PATH"setup.lua");
	}
	catch (luabind::error e)
	{
		printf("Exception: %s\n",e.what());
	}

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

	window = glfwCreateWindow(vmode->width,vmode->height,"Space Race",NULL,NULL);
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

	setupCEGUI();

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

	world = new World();
}

void destroyEngine()
{
	world->quit();
	delete world;
	if(controller)
	{
		SDL_JoystickClose(controller);
	}
	SDL_Quit();
	lua_close(luaState);
	glfwTerminate();
}

void setupCEGUI()
{
	try
	{
		//interface
		CEGUI::OpenGL3Renderer::bootstrapSystem();

		//load resources
		CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
		rp->setResourceGroupDirectory("imagesets","../../../Media/UI/datafiles/imagesets/");
		rp->setResourceGroupDirectory("fonts","../../../Media/UI/datafiles/fonts/");
		rp->setResourceGroupDirectory("layouts","../../../Media/UI/layouts/");
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
		{
			parser->setProperty("SchemaDefaultResourceGroup", "schemas");
		}

		CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
		CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-12.font");
		CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
		CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultTooltipType("TaharezLook/Tooltip");
	}
	catch(CEGUI::Exception e)
	{
		printf("CEGUI Exception: %s\n",e.what());
	}
}