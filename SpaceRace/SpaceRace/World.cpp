#include "stdafx.h"
#include "World.h"

World::World(lua_State *state)
{
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

}

void World::draw()
{

}

void World::registerWithLua(lua_State *state)
{
}