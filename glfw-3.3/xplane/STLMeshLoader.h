#pragma once
#include "mesh.h"
#include <vector>

#include "meshLoader.h"

class STLMeshLoader final: public MeshLoader
{
public:
	Mesh* load(std::string path) override;

protected:
	static void STLImport(Mesh* mesh, const std::string& fileName);

	static glm::vec3 fromBuf(char* bufptr);

	static void STLAddFace(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, glm::vec3& normal, std::vector<float>& polygons);
};
