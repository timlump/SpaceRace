// SpaceRace.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Model.h"

struct Entity
{
	Model *model;
};

struct Camera
{
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;
};

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

void logic(lua_State *luaState)
{

}

void draw(GLFWwindow *window,Shader *shader, std::vector<Entity> *entities,Camera *camera)
{
	//glFlush();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(CORN_FLOWER_BLUE);//XNA nostalgia

	glUseProgram(shader->mProgram);

	camera->model = glm::rotate(camera->model,0.1f,glm::vec3(0.0f,1.0f,0.0f));

	GLuint modelID = glGetUniformLocation(shader->mProgram,"model");
	GLuint viewID = glGetUniformLocation(shader->mProgram,"view");
	GLuint perspectiveID = glGetUniformLocation(shader->mProgram,"projection");

	glUniformMatrix4fv(modelID,1,GL_FALSE,glm::value_ptr(camera->model));
	glUniformMatrix4fv(viewID,1,GL_FALSE,glm::value_ptr(camera->view));
	glUniformMatrix4fv(perspectiveID,1,GL_FALSE,glm::value_ptr(camera->projection));

	for(int i = 0 ; i < entities->size() ; i++)
	{
		(*entities)[i].model->draw(shader);
	}

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
	//opengl
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES,4);

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *vmode = glfwGetVideoMode(monitor);

	GLFWwindow *window = glfwCreateWindow(vmode->width/2,vmode->height/2,"Space Race",NULL,NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//shader
	Shader gameShader = Shader("C:\\Users\\Timothy\\Projects\\Media\\Shaders\\test.vert","C:\\Users\\Timothy\\Projects\\Media\\Shaders\\test.frag");

	//scripting
	lua_State *luaState = luaL_newstate();
	luaL_openlibs(luaState);

	//user interface
	CEGUI::OpenGL3Renderer& guiRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();

	//sound
	irrklang::ISoundEngine *soundEngine = irrklang::createIrrKlangDevice();
	irrklang::ISound *music = soundEngine->play2D("../../../Media/Audio/MF-W-90.XM",true,false,true,irrklang::ESM_AUTO_DETECT,true);
	irrklang::ISoundEffectControl *fx = music->getSoundEffectControl();
	//fx->enableDistortionSoundEffect();

	//devil
	ilInit();

	//physics
	//to do

	//load models
	std::vector<Entity> entities;
	
	Entity entity;
	Model boxModel =  Model("../../../Media/Models/monkey.dae");
	entity.model = &boxModel;
	entities.push_back(entity);

	Camera camera;
	camera.projection = glm::perspective(67.0f,(float)vmode->width/vmode->height,0.1f,100.0f);
	camera.view = glm::lookAt(glm::vec3(0.0f,0.0f,2.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	camera.model = glm::mat4(1.0f);
	 
#pragma endregion INIT

#pragma region CORE
	//main loop
	while(!glfwWindowShouldClose(window))
	{
		updateFrameRate(window);
		logic(luaState);
		draw(window,&gameShader,&entities,&camera);
		io(window);
	}
#pragma endregion CORE

#pragma region CLEAN_UP
	soundEngine->drop();
	guiRenderer.destroySystem();
	lua_close(luaState);
	glfwTerminate();
	return 0;
#pragma endregion CLEAN_UP
}

