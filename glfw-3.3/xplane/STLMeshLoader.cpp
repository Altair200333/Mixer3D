#include "STLMeshLoader.h"
#include <fstream>

#include "fileManager.h"

Mesh *STLMeshLoader::load(std::string path)
{
	if (!FileManager::fileExists(path))
		return nullptr;

	auto mesh = new Mesh();
	STLImport(mesh, path);
	return mesh;
}
void STLMeshLoader::STLImport(Mesh *mesh, const std::string &fileName)
{
	int vertexCount = 0;
	std::vector<float> polygons;
	int nVertex = 0;
	int nFacet = 0;

	std::ifstream ifs(fileName, std::ifstream::binary);

	std::filebuf *pbuf = ifs.rdbuf();

	auto size = pbuf->pubseekoff(0, ifs.end);

	pbuf->pubseekpos(0);

	char *buffer = new char[(size_t)size];

	pbuf->sgetn(buffer, size);

	char *bufptr = buffer;

	bufptr += 80; // Skip past the header.
	bufptr += 4;  // Skip past the number of triangles.

	glm::vec3 normal;
	glm::vec3 v1, v2, v3;

	while (bufptr < buffer + size)
	{
		std::vector<float> meshList;

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

		STLAddFace(v1, v2, v3, normal, polygons);
		vertexCount += 3;

		bufptr += 2;
	}

	ifs.close();

	delete[] buffer;

	mesh->vertices = new float[vertexCount * 6];
	memcpy(&mesh->vertices[0], &polygons[0], polygons.size() * sizeof(float));

	mesh->vertexCount = vertexCount;
}
glm::vec3 STLMeshLoader::fromBuf(char *bufptr)
{
	return {*(float *)(bufptr), *(float *)(bufptr + 4), *(float *)(bufptr + 8)};
}

void STLMeshLoader::STLAddFace(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 &normal, std::vector<float> &polygons)
{
	float a[] = {
		v1.x, v1.y, v1.z, normal.x, normal.y, normal.z,
		v2.x, v2.y, v2.z, normal.x, normal.y, normal.z,
		v3.x, v3.y, v3.z, normal.x, normal.y, normal.z};

	polygons.insert(polygons.end(), a, a + 18);
}