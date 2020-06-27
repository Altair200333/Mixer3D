#pragma once
#include "mesh.h"
class VectorMath
{
public:
	static bool pointInPolygon(glm::vec3 v, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
	{
		float area = glm::length(glm::cross((v0 - v1), (v2 - v1))) * 0.5f;

		float a = glm::length(glm::cross((v - v1), (v - v2))) / (2 * area);
		float b = glm::length(glm::cross((v - v2), (v - v0))) / (2 * area);
		float c = glm::length(glm::cross((v - v1), (v - v0))) / (2 * area);

		float tresh = 1.0f + 0.0001f;
		return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
	}
	static bool pointInPolygon(glm::vec3 v, PolygonMesh p)
	{
		//bool t = (v - p.vertices[0]).magnitudeSq() < 10|| (v - p.vertices[1]).magnitudeSq() < 10|| (v - p.vertices[2]).magnitudeSq() < 10;
		float area = glm::length(glm::cross((p.vec1 - p.vec2), (p.vec3 - p.vec2))) * 0.5f;

		float a = glm::length(glm::cross((v - p.vec2), (v - p.vec3))) / (2 * area);
		float b = glm::length(glm::cross((v - p.vec3), (v - p.vec1))) / (2 * area);
		float c = glm::length(glm::cross((v - p.vec2), (v - p.vec1))) / (2 * area);

		float tresh = 1.0f + 0.0001f;
		return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
	}
	static glm::vec3 intersectPoint(glm::vec3 rayVector, glm::vec3 rayPoint, glm::vec3 planeNormal, glm::vec3 planePoint) 
	{
		glm::vec3 diff = rayPoint - planePoint;
		double prod1 = glm::dot(diff, planeNormal);
		double prod2 = glm::dot(rayVector, planeNormal);
		float ratio = prod1 / prod2;

		return rayPoint - rayVector * ratio;
	}
	static float dist2(glm::vec3 v1, glm::vec3 v2) 
	{
		return glm::length2(v1 - v2);
	}

	static glm::vec3 reflect(const glm::vec3 I, const glm::vec3 N)
	{
		return I - N * (glm::dot(I, N) * 2.0f);
	}
	static glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, const float eta_t, const float eta_i = 1.f) { // Snell's law
		float cosi = -std::max<float>(-1.f, std::min<float>(1.0f, glm::dot(I, N)));
		if (cosi < 0) return refract(I, -N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
		float eta = eta_i / eta_t;
		float k = 1 - eta * eta * (1 - cosi * cosi);
		return k < 0 ? glm::vec3(1, 0, 0) : I * eta + N * (eta * cosi - sqrtf(k)); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
	}
};