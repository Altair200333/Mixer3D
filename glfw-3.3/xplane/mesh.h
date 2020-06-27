#pragma once
#include <vector>
#include <fstream>
#include "component.h"
#include "glm/glm/glm.hpp"
#include "polygon.h"

class Mesh : public Component
{
public:
	const static uint64_t componentID = 2;
	std::shared_ptr<float[]> vertices;
	size_t vertexCount;

	Mesh(): Mesh(nullptr, 0)
	{}

	Mesh(float* _vertices, size_t vCount):vertexCount(vCount), Component(nullptr)
	{
		vertices = std::shared_ptr<float[]>(_vertices, std::default_delete<float[]>());
	}

	Polygon getPolygon(size_t number) const
	{
		return { getVertex(number*3), getVertex(number * 3 +1), getVertex(number * 3+2) };
	}
	glm::vec3 getVertex(size_t number) const
	{
		return {vertices[number * 6], vertices[number * 6 + 1], vertices[number * 6 + 2]};
	}

	glm::vec3 getNormal(size_t number) const
	{
		return {vertices[number * 6 + 3], vertices[number * 6 + 4], vertices[number * 6 + 5]};
	}
	static bool pointInPolygon(glm::vec3 v, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
	{
		//bool t = (v - p.vertices[0]).magnitudeSq() < 10|| (v - p.vertices[1]).magnitudeSq() < 10|| (v - p.vertices[2]).magnitudeSq() < 10;
		float area = glm::length(glm::cross((v0 - v1), (v2 - v1))) * 0.5f;

		float a = glm::length(glm::cross((v - v1), (v - v2))) / (2 * area);
		float b = glm::length(glm::cross((v - v2), (v - v0))) / (2 * area);
		float c = glm::length(glm::cross((v - v1), (v - v0))) / (2 * area);

		float tresh = 1.0f + 0.0001f;
		return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
	}
	static bool pointInPolygon(glm::vec3 v, Polygon& p)
	{
		//bool t = (v - p.vertices[0]).magnitudeSq() < 10|| (v - p.vertices[1]).magnitudeSq() < 10|| (v - p.vertices[2]).magnitudeSq() < 10;
		float area = glm::length(glm::cross((p.vec1 - p.vec2), (p.vec3 - p.vec2))) * 0.5f;

		float a = glm::length(glm::cross((v - p.vec2), (v - p.vec3))) / (2 * area);
		float b = glm::length(glm::cross((v - p.vec3), (v - p.vec1))) / (2 * area);
		float c = glm::length(glm::cross((v - p.vec2), (v - p.vec1))) / (2 * area);

		float tresh = 1.0f + 0.0001f;
		return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
	}

};
