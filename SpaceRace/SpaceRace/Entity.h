#include "stdafx.h"

class Entity
{
public:
	Entity(std::string luaScript)
	{
		mParent = NULL;
		mSound = NULL;
		mScript = luaScript;
	}

	enum InteractionType {KEY, COLLISION};
	struct EntityInteraction
	{
		Entity *entity;
		InteractionType type;
		std::string message;
		float value;
	};

	virtual void update(float timeStep, lua_State *state) = 0;
	virtual void interact(EntityInteraction interaction, lua_State *state) = 0;
	virtual void playSound(std::string filename) = 0;

	glm::mat4 mWorldTransform;
	irrklang::ISound *mSound;

private:
	Entity *mParent;
	std::vector<Entity*> mChildren;
	std::string mScript;
};