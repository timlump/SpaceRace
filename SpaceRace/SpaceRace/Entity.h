#include "stdafx.h"
#include "Model.h"

class Entity
{
public:
	Entity(std::string name, std::string filename, btDynamicsWorld *world, glm::vec3 position, glm::quat rotation, float mass);
	void wipeEntity(btDynamicsWorld *world);
	void draw(Shader *shader);
	void animate(float timeStep);
	void update(lua_State *state);

	void setLinearVelocity(float x, float y, float z);
	void setAngularVelocity(float x, float y, float z);

	glm::mat4 mModelMatrix;
	bool mAnimate;
	bool mLoop;
	std::string mCurrentAnimation;
	float mAnimationTime;

	std::string mUpdateLua;
	std::string mInteractLua;

	static void registerWithLua(lua_State *state);
private:
	std::string mName;
	Model *mModel;
	btRigidBody *mRigidBody;
	btCollisionShape *mCollisionShape;
};