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
	bool playMusic(std::string filename,bool forcePlay);
	void stopMusic(std::string filename);
	void host(std::string serverName, std::string mapFilename, int maxPlayers, std::string password);
	void quit();
	static void registerWithLua(lua_State *state);
private:
	void loadMap(std::string filename);
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
	irrklang::ISound *mMusic;
#pragma endregion VARIABLES
};