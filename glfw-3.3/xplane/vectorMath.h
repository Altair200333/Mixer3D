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

};