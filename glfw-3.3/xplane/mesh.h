#pragma once


#include "component.h"
#include "glm/glm/glm.hpp"
#include "polygon.h"


class Mesh : public Component
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
	
	static bool pointInPolygon(glm::vec3 v, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
	static bool pointInPolygon(glm::vec3 v, PolygonMesh& p);
	~Mesh();
};
