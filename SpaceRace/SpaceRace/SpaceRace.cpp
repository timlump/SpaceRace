// SpaceRace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Model.h"

struct Entity
{
	std::string name;
	Model *model;
	glm::mat4 scaleMatrix, rotationMatrix, translationMatrix, modelMatrix;
	btRigidBody *rigidBody;
	btCollisionShape *collisionShape;
	float time;
	bool animate;
};

struct Camera
{
	std::string name;
	glm::mat4 view;
	glm::mat4 skyBoxView;//no translations should be applied to this
	glm::mat4 projection;
	glm::vec3 position;
};

struct Light
{
	std::string name;
	glm::vec3 lightPos;
};

struct SkyBox
{
	GLuint cubeID;
};

//GLOBALS
GLFWmonitor *monitor;
GLFWwindow *window;
int width,height;

//modules
irrklang::ISoundEngine *soundEngine;
lua_State *luaState;
btBroadphaseInterface *broadphase;
btDefaultCollisionConfiguration *collisionConfiguration;
btCollisionDispatcher *dispatcher;
btSequentialImpulseConstraintSolver *solver;
btDiscreteDynamicsWorld *dynamicsWorld;

//object arrays
std::vector<Entity*> entities;
std::vector<Light*> lights;
std::vector<Camera*> cameras;

//object maps
std::map<std::string,Entity*> entityMap;
std::map<std::string,Light*> lightMap;
std::map<std::string,Camera*> cameraMap;
std::map<std::string,irrklang::ISoundSource*> soundMap;

glm::vec3 clearColor(0.0f);
Camera *currentCamera = NULL;
Shader *gameShader, *skyBoxShader;
SkyBox *skyBox = NULL;
GLuint skyVAO, skyVBO;

//functions
void initialiseEngine();
void destroyEngine();

//lua functions
void setClearColor(float r, float g, float b)
{
	clearColor.r = r;
	clearColor.g = g;
	clearColor.b = b;
}

void createEntity(std::string name, std::string filename)
{
	std::string path = MODEL_PATH + filename;
	if(entityMap.find(name)==entityMap.end())
	{
		Entity *entity = new Entity();
		Model *objectModel =  Model::loadModel(path);
		entity->name = name;
		entity->model = objectModel;
		entity->scaleMatrix = glm::mat4(1.0f);
		entity->rotationMatrix = glm::mat4(1.0f);
		entity->translationMatrix = glm::mat4(1.0f);
		entity->modelMatrix = glm::mat4(1.0f);
		entity->time = 0.0f;
		entity->animate = false;
		//create collision object

		glm::vec3 halfExtents = objectModel->mHalfExtents;
		btCollisionShape *collisionShape = new btBoxShape(btVector3(halfExtents.x,halfExtents.y,halfExtents.z));
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
		btScalar mass = 1.0f;
		btVector3 inertia(0,0,0);
		collisionShape->calculateLocalInertia(mass,inertia);
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,motionState,collisionShape,inertia);
		btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

		dynamicsWorld->addRigidBody(rigidBody);
		entity->collisionShape = collisionShape;
		entity->rigidBody = rigidBody;

		//store in map and vector
		entities.push_back(entity);
		entityMap[name] = entity;
	}
}

void createLight(std::string name,float x, float y, float z)
{
	if(lightMap.find(name)==lightMap.end())
	{
		Light *light = new Light();
		light->name = name;
		light->lightPos = glm::vec3(x,y,z);
		lights.push_back(light);
		lightMap[name] = light;
	}
}

void createCamera(std::string name, float pX, float pY, float pZ, float oX, float oY, float oZ)
{
	if(cameraMap.find(name)==cameraMap.end())
	{
		Camera *camera = new Camera();
		camera->name = name;
		camera->position = glm::vec3(pX,pY,pZ);
		camera->view = glm::lookAt(camera->position,glm::vec3(oX,oY,oZ),glm::vec3(0.0,1.0,0.0));
		camera->skyBoxView = camera->view;
		camera->projection = glm::perspective(67.0f,(float)width/height,0.1f,100.0f);
		cameras.push_back(camera);
		cameraMap[name] = camera;
	}
}

void setCurrentCamera(std::string name)
{
	if(cameraMap.find(name)!=cameraMap.end())
	{
		currentCamera = cameraMap[name];
	}
}

void createSound(std::string name, std::string filename)
{
	std::string path = AUDIO_PATH + filename;
	if(soundMap.find(name)==soundMap.end())
	{
		irrklang::ISoundSource *sound = soundEngine->addSoundSourceFromFile(path.c_str());
		soundMap[name] = sound;
	}
}

void play2DSound(std::string name, bool loop)
{
	if(soundMap.find(name)!=soundMap.end())
	{
		irrklang::ISoundSource *sound = soundMap[name];
		soundEngine->play2D(sound,loop);
	}
}

void loadSideTexture(GLuint textureID, GLenum side, std::string filename)
{
	ILuint imgID;
	ilGenImages(1,&imgID);
	ilBindImage(imgID);
	ilLoadImage((const ILstring)filename.c_str());
	ilConvertImage(IL_RGBA,IL_UNSIGNED_BYTE);

	ILubyte *data = ilGetData();
	ILuint width = ilGetInteger(IL_IMAGE_WIDTH);
	ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);

	glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);
	glTexImage2D(side,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
	ilDeleteImages(1,&imgID);
}

void setSkyBox(std::string folder)
{
	std::string path = CUBE_PATH + folder +"/";
	if(skyBox)
	{
		glDeleteTextures(1,&skyBox->cubeID);
		delete skyBox;
	}

	skyBox = new SkyBox();

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&skyBox->cubeID);

	
	//negx
	loadSideTexture(skyBox->cubeID,GL_TEXTURE_CUBE_MAP_NEGATIVE_X,path+"negx.jpg");

	//negy
	loadSideTexture(skyBox->cubeID,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,path+"negy.jpg");

	//negz
	loadSideTexture(skyBox->cubeID,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,path+"negz.jpg");

	//posx
	loadSideTexture(skyBox->cubeID,GL_TEXTURE_CUBE_MAP_POSITIVE_X,path+"posx.jpg");

	//posy
	loadSideTexture(skyBox->cubeID,GL_TEXTURE_CUBE_MAP_POSITIVE_Y,path+"posy.jpg");

	//posz
	loadSideTexture(skyBox->cubeID,GL_TEXTURE_CUBE_MAP_POSITIVE_Z,path+"posz.jpg");

	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void translateEntity(std::string name, float x, float y, float z, bool local)
{
	if(entityMap.find(name)!=entityMap.end())
	{
		Entity *entity = entityMap[name];
		if(!local)
		{
			entity->translationMatrix = glm::mat4(1.0f);
		}
		entity->translationMatrix = glm::translate(entity->translationMatrix,glm::vec3(x,y,z));
	}
}

void scaleEntity(std::string name, float x, float y, float z, bool local)
{
	if(entityMap.find(name)!=entityMap.end())
	{
		Entity *entity = entityMap[name];
		if(!local)
		{
			entity->scaleMatrix = glm::mat4(1.0f);
		}
		entity->scaleMatrix = glm::scale(entity->scaleMatrix,glm::vec3(x,y,z));
	}
}

void rotateEntity(std::string name, float x, float y, float z, float angle, bool local)
{
	if(entityMap.find(name)!=entityMap.end())
	{
		Entity *entity = entityMap[name];
		if(!local)
		{
			entity->rotationMatrix = glm::mat4(1.0f);
		}
		entity->rotationMatrix = glm::rotate(entity->rotationMatrix,angle,glm::vec3(x,y,z));
	}
}

void setWorldGravity(float x, float y, float z)
{
	dynamicsWorld->setGravity(btVector3(x,y,z));
}

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

	dynamicsWorld->stepSimulation(timeStep);

	for(int i = 0 ; i < entities.size() ; i++)
	{
		Entity *entity = entities[i];
		entity->modelMatrix = entity->scaleMatrix*entity->rotationMatrix*entity->translationMatrix;
		if(entity->animate)
		{
			entity->time += timeStep;
			entity->model->animate("",entity->time);
		}
	}

}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor.r,clearColor.g,clearColor.b,1.0f);

	if(currentCamera)
	{
		//skybox
		if(skyBox)
		{
			glDepthMask(GL_FALSE);
			skyBoxShader->bind();
			GLuint viewID = glGetUniformLocation(skyBoxShader->mProgram,"V");
			GLuint perspectiveID = glGetUniformLocation(skyBoxShader->mProgram,"P");

			currentCamera->skyBoxView = glm::rotate(currentCamera->skyBoxView,0.2f,glm::vec3(0.0f,1.0f,0.0f));

			glUniformMatrix4fv(viewID,1,GL_FALSE,glm::value_ptr(currentCamera->skyBoxView));
			glUniformMatrix4fv(perspectiveID,1,GL_FALSE,glm::value_ptr(currentCamera->projection));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP,skyBox->cubeID);
			glBindVertexArray(skyVAO);
			glDrawArrays(GL_TRIANGLES,0,36);
			glDepthMask(GL_TRUE);
			skyBoxShader->unbind();
		}

		gameShader->bind();

		GLuint viewID = glGetUniformLocation(gameShader->mProgram,"view");
		GLuint perspectiveID = glGetUniformLocation(gameShader->mProgram,"projection");
		GLuint viewPosID = glGetUniformLocation(gameShader->mProgram,"viewPos");

		glUniformMatrix4fv(viewID,1,GL_FALSE,glm::value_ptr(currentCamera->view));
		glUniformMatrix4fv(perspectiveID,1,GL_FALSE,glm::value_ptr(currentCamera->projection));
		glUniform3fv(viewPosID,1,glm::value_ptr(currentCamera->position));

		GLuint numLights = glGetUniformLocation(gameShader->mProgram,"numLights");
		glUniform1i(numLights,lights.size());

		for(int i = 0 ; i < lights.size() ; i++)
		{
			Light *light = lights[i];
			char buffer[80];
			sprintf(buffer,"lights[%d].lightPos",i);
			GLuint lightID = glGetUniformLocation(gameShader->mProgram,buffer);
			glUniform3fv(lightID,1,glm::value_ptr(light->lightPos));
		}

		for(int i = 0 ; i < entities.size() ; i++)
		{
			Entity *entity = entities[i];
			GLuint modelID = glGetUniformLocation(gameShader->mProgram,"model");
			glUniformMatrix4fv(modelID,1,GL_FALSE,glm::value_ptr(entity->modelMatrix));
			entity->model->draw(gameShader);
		}

		//draw stuff here....

		gameShader->unbind();
	}
	
	glfwSwapBuffers(window);
}

void io()
{
	glfwPollEvents();
	if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window,GL_TRUE);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	initialiseEngine();
	//scripting
	luaState = luaL_newstate();
	luaL_openlibs(luaState);
	luabind::open(luaState);

	//bind functions to lua
	luabind::module(luaState)[luabind::def("setClearColor",setClearColor)];
	luabind::module(luaState)[luabind::def("createLight",createLight)];
	luabind::module(luaState)[luabind::def("createEntity",createEntity)];
	luabind::module(luaState)[luabind::def("translateEntity",translateEntity)];
	luabind::module(luaState)[luabind::def("scaleEntity",scaleEntity)];
	luabind::module(luaState)[luabind::def("rotateEntity",rotateEntity)];
	luabind::module(luaState)[luabind::def("createCamera",createCamera)];
	luabind::module(luaState)[luabind::def("setCurrentCamera",setCurrentCamera)];
	luabind::module(luaState)[luabind::def("createSound",createSound)];
	luabind::module(luaState)[luabind::def("play2DSound",play2DSound)];
	luabind::module(luaState)[luabind::def("setSkyBox",setSkyBox)];
	luabind::module(luaState)[luabind::def("setWorldGravity",setWorldGravity)];

	//setup script
	luaL_dofile(luaState,SCRIPT_PATH"setup.lua");

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

	//irrklang
	soundEngine = irrklang::createIrrKlangDevice();

	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//shader
	gameShader = new Shader(SHADER_PATH"entityShader.vert",SHADER_PATH"entityShader.frag");
	skyBoxShader = new Shader(SHADER_PATH"skyShader.vert",SHADER_PATH"skyShader.frag");

	//setup sky box cube - based on Anton's tutorial http://antongerdelan.net/opengl/cubemaps.html
	float points[] = {
	  -10.0f,  10.0f, -10.0f,
	  -10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
	   10.0f,  10.0f, -10.0f,
	  -10.0f,  10.0f, -10.0f,
  
	  -10.0f, -10.0f,  10.0f,
	  -10.0f, -10.0f, -10.0f,
	  -10.0f,  10.0f, -10.0f,
	  -10.0f,  10.0f, -10.0f,
	  -10.0f,  10.0f,  10.0f,
	  -10.0f, -10.0f,  10.0f,
  
	   10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
   
	  -10.0f, -10.0f,  10.0f,
	  -10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f, -10.0f,  10.0f,
	  -10.0f, -10.0f,  10.0f,
  
	  -10.0f,  10.0f, -10.0f,
	   10.0f,  10.0f, -10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	  -10.0f,  10.0f,  10.0f,
	  -10.0f,  10.0f, -10.0f,
  
	  -10.0f, -10.0f, -10.0f,
	  -10.0f, -10.0f,  10.0f,
	   10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
	  -10.0f, -10.0f,  10.0f,
	   10.0f, -10.0f,  10.0f
	};
	glGenBuffers (1, &skyVBO);
	glBindBuffer (GL_ARRAY_BUFFER, skyVBO);
	glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof (float), &points, GL_STATIC_DRAW);

	glGenVertexArrays (1, &skyVAO);
	glBindVertexArray (skyVAO);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, skyVBO);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void destroyEngine()
{
	glDeleteBuffers(1,&skyVBO);
	glDeleteVertexArrays(1,&skyVAO);

	if(skyBox)
	{
		glDeleteTextures(1,&skyBox->cubeID);
		delete skyBox;
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

	for(int i = 0 ; i < entities.size() ; i++)
	{
		Entity *entity = entities[i];

		dynamicsWorld->removeRigidBody(entity->rigidBody);
		delete entity->rigidBody->getMotionState();
		delete entity->rigidBody;
		delete entity->collisionShape;

		delete entity;
	}

	for(int i = 0 ; i < lights.size() ; i++)
	{
		delete lights[i];
	}

	for(int i = 0 ; i < cameras.size() ; i++)
	{
		delete cameras[i];
	}

	entities.clear();
	lights.clear();
	cameras.clear();

	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;

	delete gameShader;
	delete skyBoxShader;

	soundEngine->drop();
	lua_close(luaState);
	glfwTerminate();
}