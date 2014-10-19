#define WORLD_H

#include "stdafx.h"
#include "Model.h"
#ifndef ENTITY_H
#include "Entity.h"
#endif
#include "Camera.h"


enum InputType{JOY_X, JOY_Y, MOUSE_X, MOUSE_Y, KEY};
enum KeyType{FORWARD,BACKWARD,LEFT,RIGHT,
	LEAN_LEFT,LEAN_RIGHT,
	JUMP,CROUCH,PRONE,SPIN_180,
	FIRE,MELEE,RELOAD,AIM,
	FLASHLIGHT,USE,FREELOOK,TALK,
	SLOT_0,SLOT_1,SLOT_2,SLOT_3,SLOT_4,SLOT_5,
	SLOT_6,SLOT_7,SLOT_8,
	SLOT_UP,SLOW_DOWN,CONFIRM};
enum KeyAction{PRESS,RELEASE};

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