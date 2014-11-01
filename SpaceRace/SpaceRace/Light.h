#include "stdafx.h"

class Light
{
public:
	Light(glm::vec3 pos, glm::vec3 colour)
	{
		mPos = pos;
		mColour = colour;
	}
	glm::vec3 mPos;
	glm::vec3 mColour;
private:
};