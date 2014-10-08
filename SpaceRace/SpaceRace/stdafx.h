// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define CORN_FLOWER_BLUE 0.39f,0.58f,0.92f,1.0f
#define FPS_TRACK_DELAY 0.25
#define MAX_LIGHTS 64

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include <lua.hpp>

#include <GL\glew.h>
#define GLFW_DLL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

#include <CEGUI\RendererModules\OpenGL\GL3Renderer.h>
#include <CEGUI\System.h>

#include <irrKlang.h>

#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>

#include <btBulletDynamicsCommon.h>

#include <assert.h>


// TODO: reference additional headers your program requires here
