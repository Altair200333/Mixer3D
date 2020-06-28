#pragma once
#include <glm/glm/glm.hpp>

struct PolygonMesh final
{
	PolygonMesh(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) : vec1(v1), vec2(v2), vec3(v3), normal({1,0,0}) {}
	PolygonMesh(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 _n) : vec1(v1), vec2(v2), vec3(v3), normal(_n) {}

	glm::vec3 vec1;
	glm::vec3 vec2;
	glm::vec3 vec3;
	glm::vec3 normal;

	//max distance between vertices
	float maxDistance = -1;
};
