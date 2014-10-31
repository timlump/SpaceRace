#include "stdafx.h"
#include "World.h"

World::World()
{
	mUpdateScript = "";
	mShader = new Shader(SHADER_PATH"entityShader.vert",SHADER_PATH"entityShader.frag");
	mLuaState = NULL; //don't own this
	mSoundEngine = irrklang::createIrrKlangDevice();
	mMusic = NULL;
	mBroadphase = new btDbvtBroadphase();
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mSolver = new btSequentialImpulseConstraintSolver;
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
	for(int i = 0 ; i < 30 ; i++)
	{
		mKeyPressed[i] = false;
	}
}

World::~World()
{
	delete mDynamicsWorld;
	delete mSolver;
	delete mDispatcher;
	delete mCollisionConfiguration;
	delete mBroadphase;
	delete mSoundEngine;
	delete mShader;
}

void World::update(float timeStep)
{
	if(mLuaState && mUpdateScript.empty() == false)
	{
		try
		{
			luabind::call_function<void>(mLuaState,"dofile",mUpdateScript);
		}
		catch (luabind::error e)
		{
			printf("Exception: %s\n",e.what());
		}
	}

	std::vector<Entity*>::iterator it;
	for(it = mEntities.begin() ; it!=mEntities.end() ; it++)
	{
		(*it)->update(timeStep,mLuaState);
	}
}

void World::draw()
{
	std::vector<Entity*>::iterator it;
	for(it = mEntities.begin() ; it!= mEntities.end() ; it++)
	{
		(*it)->draw(mShader);
	}
}

void World::control(InputType type, KeyType key, KeyAction action)
{
	if(type == InputType::KEY)
	{
		printf("%d\n",key);
		mKeyPressed[key] = (action==KeyAction::PRESS)?true:false;
	}
}

void World::control(InputType type, float value)
{
	switch(type)
	{
	case InputType::JOY_X:
		printf("%f\n",value);
		break;
	case InputType::JOY_Y:
		break;
	case InputType::MOUSE_X:
		break;
	case InputType::MOUSE_Y:
		break;
	}
}

bool World::playMusic(std::string filename,bool forcePlay)
{
	std::string fullPath = AUDIO_PATH + filename;
	if(mSoundEngine)
	{
		if(mMusic)
		{
			if(mMusic->isFinished())
			{
				mMusic->drop();
				mMusic = mSoundEngine->play2D(fullPath.c_str(),true,false,true);
				return true;
			}
			else if (forcePlay)
			{
				mMusic->stop();
				mMusic->drop();
				mMusic = mSoundEngine->play2D(fullPath.c_str(),true,false,true);
				return true;
			}
		}
		else
		{
			mMusic = mSoundEngine->play2D(fullPath.c_str(),true,false,true);
			return true;
		}
	}
	return false;
}

void World::setWorldColour(float r, float g, float b)
{
	glClearColor(r,g,b,1.0f);
}

float World::getTime()
{
	return (float)glfwGetTime();
}

void World::host(std::string serverName, std::string mapFilename, int maxPlayers, std::string password)
{
	printf("Server Name: %s, Map Name: %s, Max Players: %d, Password: %s\n",serverName.c_str(),mapFilename.c_str(),maxPlayers,password.c_str());
	loadMap(MAP_PATH+mapFilename);
}

void World::quit()
{

}

void World::stopMusic()
{	
	if(mMusic)
	{
		mMusic->stop();
		mMusic->drop();
		mMusic = NULL;
	}
}

void World::registerWithLua(lua_State *state)
{
	luabind::module(state)
		[
			luabind::class_<World>("World")
			.def("playMusic",&World::playMusic)
			.def("stopMusic",&World::stopMusic)
			.def("setWorldColour",&World::setWorldColour)
			.def("getTime",&World::getTime)
		];
}

void World::setLuaState(lua_State *state, std::string updateScript)
{
	mLuaState = state;
	mUpdateScript = updateScript;
}

void World::loadMap(std::string filename)
{

}