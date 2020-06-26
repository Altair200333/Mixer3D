#pragma once
#include "scene.h"
#include "libbitmap.h"
class RenderEngine
{
public:
	virtual ~RenderEngine() = default;
	RenderEngine() = default;
	
	virtual Bitmap render(Scene& scene, int width, int height) = 0;
};
