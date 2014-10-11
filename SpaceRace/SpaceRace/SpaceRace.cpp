// SpaceRace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Model.h"



struct Entity
{
	std::string name;
	Model *model;
	glm::mat4 modelMatrix;
	btRigidBody *rigidBody;
	float time;
};

struct Camera
{
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 position;
};

struct Light
{
	glm::vec3 lightPos;
};

//GLOBALS
glm::vec3 clearColor(0.0f);
Camera camera;
GLFWmonitor *monitor;
GLFWwindow *window;
int width,height;
std::vector<Entity*> entities;
std::vector<Light> lights;
irrklang::ISoundEngine *soundEngine;
Shader *gameShader;
btBroadphaseInterface *broadphase;
btDefaultCollisionConfiguration *collisionConfiguration;
btCollisionDispatcher *dispatcher;
btSequentialImpulseConstraintSolver *solver;
btDiscreteDynamicsWorld *dynamicsWorld;

//lua functions
void setClearColor(float r, float g, float b)
{
	clearColor.r = r;
	clearColor.g = g;
	clearColor.b = b;
}
void initializeEngine(bool fullScreen, int AASamples)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES,AASamples);

	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *vmode = glfwGetVideoMode(monitor);

	width = vmode->width;
	height = vmode->height;

	window = glfwCreateWindow(vmode->width,vmode->height,"Space Race",(fullScreen)?monitor:NULL,NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//devil
	ilInit();

	//irrklang
	soundEngine = irrklang::createIrrKlangDevice();
	//irrklang::ISound *music = soundEngine->play2D("../../../Media/Audio/MF-W-90.XM",true,false,true,irrklang::ESM_AUTO_DETECT,true);
	//irrklang::ISoundEffectControl *fx = music->getSoundEffectControl();
	//fx->enableDistortionSoundEffect();

	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//shader
	gameShader = new Shader("../../../Media/Shaders/entityShader.vert","../../../Media/Shaders/entityShader.frag");
}
void setupPerspectiveMatrix(float fov, float nearP, float farP)
{
	camera.projection = glm::perspective(fov,(float)width/height,nearP,farP);
}
void setupViewMatrix(float pX, float pY, float pZ, float oX, float oY, float oZ, float uX, float uY, float uZ)
{
	camera.view = glm::lookAt(glm::vec3(pX,pY,pZ),glm::vec3(oX,oY,oZ),glm::vec3(uX,uY,uZ));
}
void loadEntity(std::string name, std::string filename)
{
	Entity *entity = new Entity();
	Model *objectModel =  Model::loadModel(filename);
	entity->name = name;
	entity->model = objectModel;
	entity->modelMatrix = glm::mat4(1.0f);
	entity->time = 0.0f;
	entities.push_back(entity);
}
void createLight(float x, float y, float z)
{
	Light light;
	light.lightPos = glm::vec3(x,y,z);
	lights.push_back(light);
}

//Anton's OpenGL 4 Tutorials - http://antongerdelan.net/opengl/
void updateFrameRate(GLFWwindow *window)
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

void logic(std::vector<Entity*> *entities, lua_State *luaState, btDiscreteDynamicsWorld *dynamicsWorld)
{
	float timeStep = getTimeStep();

	dynamicsWorld->stepSimulation(timeStep);

	for(int i = 0 ; i < entities->size() ; i++)
	{
		Entity *entity = (*entities)[i];
		entity->time += timeStep;
		entity->model->animate("",entity->time);
	}

}

void draw(GLFWwindow *window,Shader *shader, std::vector<Entity*> *entities,std::vector<Light> *lights,Camera *camera)
{
	//glFlush();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor.r,clearColor.g,clearColor.b,1.0f);

	shader->bind();

	//camera->model = glm::rotate(camera->model,0.1f,glm::vec3(0.0f,1.0f,0.0f));

	GLuint viewID = glGetUniformLocation(shader->mProgram,"view");
	GLuint perspectiveID = glGetUniformLocation(shader->mProgram,"projection");
	GLuint viewPosID = glGetUniformLocation(shader->mProgram,"viewPos");

	glUniformMatrix4fv(viewID,1,GL_FALSE,glm::value_ptr(camera->view));
	glUniformMatrix4fv(perspectiveID,1,GL_FALSE,glm::value_ptr(camera->projection));
	glUniform3fv(viewPosID,1,glm::value_ptr(camera->position));

	GLuint numLights = glGetUniformLocation(shader->mProgram,"numLights");
	glUniform1i(numLights,lights->size());

	for(int i = 0 ; i < lights->size() ; i++)
	{
		char buffer[80];
		sprintf(buffer,"lights[%d].lightPos",i);
		GLuint lightID = glGetUniformLocation(shader->mProgram,buffer);
		glUniform3fv(lightID,1,glm::value_ptr((*lights)[i].lightPos));
	}

	for(int i = 0 ; i < entities->size() ; i++)
	{
		GLuint modelID = glGetUniformLocation(shader->mProgram,"model");
		glUniformMatrix4fv(modelID,1,GL_FALSE,glm::value_ptr((*entities)[i]->modelMatrix));
		(*entities)[i]->model->draw(shader);
	}

	shader->unbind();

	//draw stuff here....
	
	//render the user interface
	CEGUI::System::getSingleton().renderAllGUIContexts();
	glfwSwapBuffers(window);
}

void io(GLFWwindow *window)
{
	glfwPollEvents();
	if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window,GL_TRUE);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
#pragma region INIT
	//scripting
	lua_State *luaState = luaL_newstate();
	luaL_openlibs(luaState);
	luabind::open(luaState);

	//bind functions to lua
	luabind::module(luaState)[luabind::def("initializeEngine",initializeEngine)];
	luabind::module(luaState)[luabind::def("setClearColor",setClearColor)];
	luabind::module(luaState)[luabind::def("setupPerspectiveMatrix",setupPerspectiveMatrix)];
	luabind::module(luaState)[luabind::def("setupViewMatrix",setupViewMatrix)];
	luabind::module(luaState)[luabind::def("loadEntity",loadEntity)];
	luabind::module(luaState)[luabind::def("createLight",createLight)];

	//setup script
	luaL_dofile(luaState,"../../../Media/Scripts/setup.lua");

	//user interface
	CEGUI::OpenGL3Renderer& guiRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();
	
	 
#pragma endregion INIT

#pragma region CORE
	//main loop
	while(!glfwWindowShouldClose(window))
	{
		updateFrameRate(window);
		logic(&entities,luaState,dynamicsWorld);
		draw(window,gameShader,&entities,&lights,&camera);
		io(window);
	}
#pragma endregion CORE

#pragma region CLEAN_UP

	for(int i = 0 ; i < entities.size() ; i++)
	{
		delete entities[i];
		entities.clear();
	}

	std::map<std::pair<GLuint,GLuint>,GLuint>::iterator it;
	for(it = Shader::mPrograms.begin() ; it!=Shader::mPrograms.end() ; it++)
	{
		glDeleteProgram(it->second);
	}

	static std::map<std::string,GLuint>::iterator it2;
	for(it2 = Shader::mVertShaders.begin() ; it2!= Shader::mVertShaders.end() ; it2++)
	{
		glDeleteShader(it2->second);
	}

	for(it2 = Shader::mFragShaders.begin() ; it2!= Shader::mFragShaders.end() ; it2++)
	{
		glDeleteShader(it2->second);
	}

	std::map<std::string,GLuint>::iterator it3;
	for(it3 = Model::mTextureIDs.begin() ; it3!=Model::mTextureIDs.end() ; it3++)
	{
		glDeleteTextures(1,&it3->second);
	}

	std::map<std::string,Model*>::iterator it4;
	for(it4 = Model::mModels.begin() ; it4!=Model::mModels.end() ; it4++)
	{
		it4->second->wipeModel();
		delete it4->second;
	}

	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;

	delete gameShader;

	soundEngine->drop();
	guiRenderer.destroySystem();
	lua_close(luaState);
	glfwTerminate();
	return 0;
#pragma endregion CLEAN_UP
}

