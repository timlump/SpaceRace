#include "stdafx.h"
#include "Entity.h"

Entity::Entity(std::string name, std::string filename, btDynamicsWorld *world, glm::vec3 position, glm::quat rotation, float mass)
{
	mAnimate = false;
	mLoop = true;
	mCurrentAnimation = "";
	mUpdateLua = "";
	mInteractLua = "";

	mAnimationTime = 0.0f;
	mModelMatrix = glm::mat4(1.0f);
	mName = name;

	mModel = Model::loadModel(filename);
	glm::vec3 halfExtents = mModel->mHalfExtents;
	mCollisionShape = new btBoxShape(btVector3(halfExtents.x,halfExtents.y,halfExtents.z));
	btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(rotation.x,rotation.y,rotation.z,rotation.w),btVector3(position.x,position.y,position.z)));
	btVector3 inertia(0,0,0);
	mCollisionShape->calculateLocalInertia(mass,inertia);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,motionState,mCollisionShape,inertia);
	mRigidBody = new btRigidBody(rigidBodyCI);

	world->addRigidBody(mRigidBody);
}

void Entity::wipeEntity(btDynamicsWorld *world)
{
	world->removeRigidBody(mRigidBody);
	delete mRigidBody->getMotionState();
	delete mRigidBody;
	delete mCollisionShape;
}

void Entity::animate(float timeStep)
{
	//calculate model matrix
	btTransform modelMatrix = mRigidBody->getWorldTransform();
	modelMatrix.getOpenGLMatrix(glm::value_ptr(mModelMatrix));

	//animate local space
	if(mAnimate)
	{
		mAnimationTime += timeStep;
		mModel->animate(mCurrentAnimation,mAnimationTime,mLoop);
	}
}

void Entity::draw(Shader *shader)
{
	GLuint modelID = glGetUniformLocation(shader->mProgram,"model");
	glUniformMatrix4fv(modelID,1,GL_FALSE,glm::value_ptr(mModelMatrix));
	mModel->draw(shader);
}

void Entity::setLinearVelocity(float x, float y, float z)
{
	mRigidBody->setLinearVelocity(btVector3(x,y,z));
}

void Entity::setAngularVelocity(float x, float y, float z)
{
	mRigidBody->setAngularVelocity(btVector3(x,y,z));
}

void Entity::update(lua_State *state)
{
	if(!mUpdateLua.empty())
	{
		try
		{
				std::string path = SCRIPT_PATH+mUpdateLua;
				luaL_dofile(state,path.c_str());
				luabind::call_function<void>(state,"update",this);
		}
		catch (luabind::error e)
		{
			printf("Exception: %s\n",e.what());
		}
	}
}

void Entity::registerWithLua(lua_State *state)
{
	luabind::module(state)
		[
			luabind::class_<Entity>("Entity")
			.def("setLinearVelocity",&Entity::setLinearVelocity)
			.def("setAngularVelocity",&Entity::setAngularVelocity)
			.def_readonly("mName",&Entity::mName)
			.def_readwrite("mAnimationTime",&Entity::mAnimationTime)
			.def_readwrite("mAnimate",&Entity::mAnimate)
			.def_readwrite("mLoop",&Entity::mLoop)
			.def_readwrite("mCurrentAnimation",&Entity::mCurrentAnimation)
			.def_readwrite("mInteractLua",&Entity::mInteractLua)
			.def_readwrite("mUpdateLua",&Entity::mUpdateLua)
		];
}

