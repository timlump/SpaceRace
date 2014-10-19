#define WORLD_H

#include "stdafx.h"
#include "Model.h"
#ifndef ENTITY_H
#include "Entity.h"
#endif
#include "Camera.h"

class World
{
public:
	World(lua_State *state);
	~World();
	void update(float timeStep);
	void draw();
	void control(InputType type, KeyType key, KeyAction action);
	void control(InputType type, float value);

	static void registerWithLua(lua_State *state);
private:
#pragma region MODULES
	lua_State *mLuaState;
	irrklang::ISoundEngine *mSoundEngine;
	btBroadphaseInterface *mBroadphase;
	btDefaultCollisionConfiguration *mCollisionConfiguration;
	btCollisionDispatcher *mDispatcher;
	btSequentialImpulseConstraintSolver *mSolver;
	btDiscreteDynamicsWorld *mDynamicsWorld;
#pragma endregion MODULES
#pragma region VARIABLES
	Shader *mShader;
	std::vector<Entity*> mEntities;
	Camera *mCamera;
	bool mKeyPressed[30];
#pragma endregion VARIABLES
};