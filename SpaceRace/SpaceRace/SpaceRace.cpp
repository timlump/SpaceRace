// SpaceRace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "World.h"

//GLOBALS
GLFWmonitor *monitor = NULL;
GLFWwindow *window = NULL;
int width,height;

//modules
lua_State *luaState = NULL;
TwBar *bar = NULL;
World *world = NULL;

SDL_Joystick* controller = NULL;

//functions
void initialiseEngine();
void destroyEngine();

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
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	world->draw();
	
	glfwSwapBuffers(window);
}

void io()
{
	glfwPollEvents();
	if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window,GL_TRUE);
	}

	if(controller)
	{
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
						world->control(InputType::JOY_X,event.jaxis.value);
					}
					else if(event.jaxis.axis == 1)
					{
						world->control(InputType::JOY_Y,event.jaxis.value);
					}
				}
				break;
			case SDL_JOYBUTTONDOWN:
				//printf("Down: %d\n",event.jbutton.button);
				break;
			case SDL_JOYBUTTONUP:
				//printf("Up: %d\n",event.jbutton.button);
				break;
			}
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

	window = glfwCreateWindow(vmode->width,vmode->height,"Space Race",NULL,NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//devil
	ilInit();

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