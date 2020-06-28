#pragma once
#include "material.h"
#include "glm/glm/glm.hpp"

struct Hit final
{
    glm::vec3 pos;
    glm::vec3 normal;
    Material* material;
    bool hit;

    Hit(glm::vec3 _pos, glm::vec3 _norm, Material* _material, bool _hit) : pos(_pos), normal(_norm), material(_material), hit(_hit)
    {
    }
};
