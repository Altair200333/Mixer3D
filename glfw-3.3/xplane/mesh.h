#pragma once


#include "component.h"
#include "glm/glm/glm.hpp"
#include "polygon.h"


class Mesh final: public Component
{
public:
	const static uint64_t componentID = 2;
	float* vertices;
	size_t vertexCount;

	Mesh(): Mesh(nullptr, 0)
	{}

	Mesh(float* _vertices, size_t vCount);

	glm::vec3 getVertex(size_t number) const;
	
	PolygonMesh getPolygon(size_t number) const;
	
	glm::vec3 getNormal(size_t number) const;
	
	~Mesh();
};
