#include "stdafx.h"

class Camera
{
public:
	Camera(float fov, float aspect, float nearPlane, float farPlane);
	~Camera();
	glm::mat4 mProjection,mView;
private:
	float mFov, mAspect, mNearPlane, mFarPlane;
};