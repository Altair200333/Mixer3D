#include "mesh.h"

Mesh::Mesh(float* _vertices, size_t vCount) :vertexCount(vCount), Component(nullptr)
{
	vertices = _vertices;
}

glm::vec3 Mesh::getVertex(size_t number) const
{
	return { vertices[number * 6], vertices[number * 6 + 1], vertices[number * 6 + 2] };
}

PolygonMesh Mesh::getPolygon(size_t number) const
{
	return PolygonMesh(getVertex(number * 3), getVertex(number * 3 + 1), getVertex(number * 3 + 2));
}
glm::vec3 Mesh::getNormal(size_t number) const
{
	return { vertices[number * 6 + 3], vertices[number * 6 + 4], vertices[number * 6 + 5] };
}


Mesh::~Mesh()
{
	delete vertices;
}
