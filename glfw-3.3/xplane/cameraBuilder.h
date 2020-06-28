#pragma once
#include "camera.h"
#include "objectBuilder.h"

class CameraBuilder:public ObjectBuilder
{
public:
	CameraBuilder() :ObjectBuilder() {}

	CameraBuilder& addCamera(float ar, float fov)
	{
		obj->addComponent(new Camera(obj, ar, fov));
		return *this;
	}
};