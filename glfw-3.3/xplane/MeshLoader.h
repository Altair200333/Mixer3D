#pragma once
#include "mesh.h"
class MeshLoader
{
public:
	virtual Mesh* load(std::string path) = 0;
};