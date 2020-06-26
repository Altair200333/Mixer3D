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

	Mesh(const std::string& fname, Object* _owner):Component(_owner)
	{
		STLImport(fname);
	}
	
	void STLImport(const std::string& fileName)
	{
		vertexCount = 0;
		std::vector<float> polygons;
		int nVertex = 0;
		int nFacet = 0;

		std::ifstream ifs(fileName, std::ifstream::binary);

		std::filebuf* pbuf = ifs.rdbuf();

		auto size = pbuf->pubseekoff(0, ifs.end);

		pbuf->pubseekpos(0);

		char* buffer = new char[(size_t)size];

		pbuf->sgetn(buffer, size);

		char* bufptr = buffer;

		bufptr += 80; // Skip past the header.
		bufptr += 4; // Skip past the number of triangles.

		glm::vec3 normal;
		glm::vec3 v1, v2, v3;

		while (bufptr < buffer + size)
		{
			normal = fromBuf(bufptr);
			bufptr += 12;

			v1 = fromBuf(bufptr);
			bufptr += 12;

			v2 = fromBuf(bufptr);
			bufptr += 12;

			v3 = fromBuf(bufptr);
			bufptr += 12;

			const float eps = (float)1.0e-9;

			// If the normal in the STL file is blank, then create a proper normal.
			if (abs(normal.x) < eps && abs(normal.y) < eps && abs(normal.z) < eps)
			{
				glm::vec3 u, v;
				u = v2 - v1;
				v = v3 - v1;
				normal = glm::cross(u, v);
				normal = glm::normalize(normal);
			}

			nFacet++;
			nVertex += 3;

			STLAddFacet(v1, v2, v3, normal, polygons);

			bufptr += 2;
		}

		ifs.close();

		delete[] buffer;

		vertices = std::shared_ptr<float[]>(new float[vertexCount * 6], std::default_delete<float[]>());
		memcpy(&vertices[0], &polygons[0], polygons.size() * sizeof(float));
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
	static bool pointInPolygon(glm::vec3 v, Polygon p)
	{
		//bool t = (v - p.vertices[0]).magnitudeSq() < 10|| (v - p.vertices[1]).magnitudeSq() < 10|| (v - p.vertices[2]).magnitudeSq() < 10;
		float area = glm::length(glm::cross((p.vec1 - p.vec2), (p.vec3 - p.vec2))) * 0.5f;

		float a = glm::length(glm::cross((v - p.vec2), (v - p.vec3))) / (2 * area);
		float b = glm::length(glm::cross((v - p.vec3), (v - p.vec1))) / (2 * area);
		float c = glm::length(glm::cross((v - p.vec2), (v - p.vec1))) / (2 * area);

		float tresh = 1.0f + 0.0001f;
		return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
	}

protected:
	
	glm::vec3 fromBuf(char* bufptr)
	{
		return {*(float*)(bufptr), *(float*)(bufptr + 4), *(float*)(bufptr + 8)};
	}

	void STLAddFacet(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 normal, std::vector<float>& polygons)
	{
		float a[] = {
			v1.x, v1.y, v1.z, normal.x, normal.y, normal.z,
			v2.x, v2.y, v2.z, normal.x, normal.y, normal.z,
			v3.x, v3.y, v3.z, normal.x, normal.y, normal.z
		};

		polygons.insert(polygons.end(), a, a + 18);

		vertexCount += 3;
	}

};
