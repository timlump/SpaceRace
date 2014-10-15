#include "stdafx.h"
#include "Model.h"
#include "Entity.h"

class World
{
public:
	World(lua_State *state);
	~World();
	void update(float timeStep);
	void draw();

	/*void addCamera(Camera *camera);
	void removeCamera(Camera *camera);

	void addLight(Light *light);
	void removeLight(Light *light);

	void addCharacter(Character *character);
	void removeCharacter(Character *character);*/

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
	std::vector<Entity*> mEntities;

	//std::map<std::string,Entity*> mEntityMap;
	//std::map<std::string,Light*> mLightMap;
	//std::map<std::string,Camera*> mCameraMap;
	//std::map<std::string,irrklang::ISoundSource*> mSoundMap;
#pragma endregion VARIABLES
};