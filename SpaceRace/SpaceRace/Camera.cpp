#include "stdafx.h"
#include "Camera.h"

Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
{
	mProjection = glm::perspective(fov,aspect,nearPlane,farPlane);
	mView = glm::lookAt(glm::vec3(0.0f),glm::vec3(0.0f,0.0f,-1.0f),glm::vec3(0.0f,1.0f,0.0f));
}