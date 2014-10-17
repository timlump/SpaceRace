#include "stdafx.h"
#include "World.h"

World::World(lua_State *state)
{
	mShader = NULL;
	mLuaState = state; //don't own this
	mSoundEngine = irrklang::createIrrKlangDevice();
	mBroadphase = new btDbvtBroadphase();
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mSolver = new btSequentialImpulseConstraintSolver;
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
}

World::~World()
{
	delete mDynamicsWorld;
	delete mSolver;
	delete mDispatcher;
	delete mCollisionConfiguration;
	delete mBroadphase;
	delete mSoundEngine;
}

void World::update(float timeStep)
{
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

void World::control(InputType type, int value)
{

}

void World::registerWithLua(lua_State *state)
{

}