#include "vectorMath.h"
#include "glm/glm/gtx/norm.hpp"
#include <algorithm>

bool VectorMath::pointInPolygon(glm::vec3 v, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	float area = glm::length(glm::cross((v0 - v1), (v2 - v1))) * 0.5f;

	float a = glm::length(glm::cross((v - v1), (v - v2))) / (2 * area);
	float b = glm::length(glm::cross((v - v2), (v - v0))) / (2 * area);
	float c = glm::length(glm::cross((v - v1), (v - v0))) / (2 * area);

	float tresh = 1.0f + 0.0001f;
	return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
}
bool VectorMath::pointInPolygon(glm::vec3 v, const PolygonMesh& p)
{
	float area = glm::length(glm::cross((p.vec1 - p.vec2), (p.vec3 - p.vec2))) * 0.5f;

	float a = glm::length(glm::cross((v - p.vec2), (v - p.vec3))) / (2 * area);
	float b = glm::length(glm::cross((v - p.vec3), (v - p.vec1))) / (2 * area);
	float c = glm::length(glm::cross((v - p.vec2), (v - p.vec1))) / (2 * area);

	float tresh = 1.0f + 0.0001f;
	return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
}
glm::vec3 VectorMath::intersectPoint(const glm::vec3& rayVector, const glm::vec3& rayPoint, const glm::vec3& planeNormal, const glm::vec3& planePoint)
{
	glm::vec3 diff = rayPoint - planePoint;
	double prod1 = glm::dot(diff, planeNormal);
	double prod2 = glm::dot(rayVector, planeNormal);
	float ratio = prod1 / prod2;

	return rayPoint - rayVector * ratio;
}
float VectorMath::dist2(glm::vec3 v1, glm::vec3 v2)
{
	return glm::length2(v1 - v2);
}

glm::vec3 VectorMath::reflect(const glm::vec3& I, const glm::vec3& N)
{
	return I - N * (glm::dot(I, N) * 2.0f);
}
glm::vec3 VectorMath::refract(const glm::vec3& I, const glm::vec3& N, const float eta_t, const float eta_i)
{
	float cosi = -std::max<float>(-1.f, std::min<float>(1.0f, glm::dot(I, N)));
	if (cosi < 0) return refract(I, -N, eta_i, eta_t);
	float eta = eta_i / eta_t;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? reflect(I, N) : I * eta + N * (eta * cosi - sqrtf(k));
}