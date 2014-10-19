#include "stdafx.h"

#define ENTITY_H

#ifndef SHADER_H
#include "Shader.h"
#endif

#ifndef WORLD_H
#include "World.h"
#endif

class Entity;

enum InteractionType {USER, COLLISION};
struct EntityInteraction
{
	Entity *entity;
	InteractionType type;
	//KeyType key;
	std::string message;
	float value;
};

class Entity
{
public:
	Entity(std::string luaScript)
	{
		mParent = NULL;
		mSound = NULL;
		mEngine = NULL;
		mScript = luaScript;
	}

	bool playSound(std::string filename, bool loop, bool forcePlay)
	{
		if(mEngine)
		{
			//get sound position
			glm::vec3 pos(mWorldTransform[3]);
			irrklang::vec3df soundPos(pos.x,pos.y,pos.z);

			if(mSound)
			{
				//check if playing
				if(mSound->isFinished())
				{
					mSound->drop();
					mSound = mEngine->play3D(filename.c_str(),soundPos,loop);
					return true;
				}
				else if (forcePlay)
				{
					mSound->stop();
					mSound->drop();
					mSound = mEngine->play3D(filename.c_str(),soundPos,loop);
					return true;
				}
			}
			else
			{
				mSound = mEngine->play3D(filename.c_str(),soundPos,loop);
				return true;
			}
		}
		return false;
	}
	bool stopSound()
	{
		if(mSound)
		{
			mSound->stop();
			mSound->drop();
		}
	}

	void initialise(lua_State *state)
	{
		if(!mScript.empty())
		{
			try
			{
				luaL_dofile(state,mScript.c_str());
				luabind::call_function<void>(state,"init");
			}
			catch(luabind::error e)
			{
				printf("Lua Expection: %s\n",e.what());
			}
		}
	}
	void update(float timeStep, lua_State *state)
	{
		if(mEngine && mSound)
		{
			if(!mSound->isFinished())
			{
				glm::vec3 pos(mWorldTransform[3]);
				irrklang::vec3df soundPos(pos.x,pos.y,pos.z);
				mSound->setPosition(soundPos);
			}
		}
	}

	void addChild(Entity *child)
	{
		mChildren.push_back(child);
	}
	void removeChild(Entity *child)
	{
		mChildren.erase(std::remove(mChildren.begin(),mChildren.end(),child),mChildren.end());
	}
	void setParent(Entity *parent)
	{
		mParent = parent;
	}
	void clearParent()
	{
		mParent = NULL;
	}

	virtual void draw(Shader *shader) = 0;
	virtual void interact(EntityInteraction interaction, lua_State *state) = 0;

	glm::mat4 mWorldTransform;
	irrklang::ISound *mSound;
	irrklang::ISoundEngine *mEngine;
private:
	Entity *mParent;
	std::vector<Entity*> mChildren;
	std::string mScript;
};