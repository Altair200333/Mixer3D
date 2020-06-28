#pragma once
#include "mesh.h"
class VectorMath
{
public:
	static bool pointInPolygon(glm::vec3 v, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
	static bool pointInPolygon(glm::vec3 v, const PolygonMesh& p);
	static glm::vec3 intersectPoint(const glm::vec3& rayVector, const glm::vec3& rayPoint, const glm::vec3& planeNormal, const glm::vec3& planePoint);
	static float dist2(glm::vec3 v1, glm::vec3 v2);

	static glm::vec3 reflect(const glm::vec3& I, const glm::vec3& N);
	static glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, const float eta_t, const float eta_i = 1.f);
};