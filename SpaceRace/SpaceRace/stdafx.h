// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define NOMINMAX
#define CORN_FLOWER_BLUE 0.39f,0.58f,0.92f,1.0f
#define FPS_TRACK_DELAY 0.25
#define MAX_LIGHTS 64
const int DEADZONE = 2000;
const int AXIS_RANGE = 32767;

#define MEDIA_PATH "../../../Media/"
#define MODEL_PATH "../../../Media/Models/"
#define SHADER_PATH "../../../Media/Shaders/"
#define AUDIO_PATH "../../../Media/Audio/"
#define SCRIPT_PATH "../../../Media/Scripts/"
#define CUBE_PATH "../../../Media/CubeMaps/"
#define LAYOUT_PATH "../../../Media/UI/"
#define KEYBIND_PATH "../../../Media/"
#define MAP_PATH "../../../Media/Maps/"
#define MAP_FORMAT ".sr_xml"

#include "targetver.h"

#include <AntTweakBar.h>

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include <lua.hpp>
#include <luabind\luabind.hpp>

#include <GL\glew.h>

#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

#include <CEGUI\RendererModules\OpenGL\GL3Renderer.h>
#include <CEGUI\CEGUI.h>

#include <irrKlang.h>

#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>

#include <btBulletDynamicsCommon.h>

#include <SDL.h>

#include <assert.h>


// TODO: reference additional headers your program requires here
