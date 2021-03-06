#pragma once
#include <corecrt_math_defines.h>
#include <thread>
#include <glm/glm/gtx/norm.hpp>


#include "bmpWriter.h"
#include "LightSource.h"
#include "mesh.h"
#include "renderEngine.h"
#include "vectorMath.h"
#include "hit.h"
class RayTracingRenderer final: public RenderEngine
{
protected:
    
    struct OptiMesh final
    {
        glm::vec3 pos;
        std::vector<PolygonMesh> polygons;
        Material* mat;
    };
    struct OptiLight final
    {
        OptiLight(glm::vec3 _pos, PointLight* pl) :pos(_pos), light(pl) {}

        glm::vec3 pos;
        PointLight* light;
    };
	
    int width, height;
    std::vector<OptiMesh> meshes;
    std::vector<OptiLight> lights;
public:
    Bitmap* env;
	~RayTracingRenderer() = default;

    
    Bitmap render(Scene& scene, int width, int height) override;
protected:
    void batchSceneMeshes(Scene& scene);
    void batchLight(Scene& scene);
    void renderWithTracing(Scene& scene, Bitmap& img);
	
    void renderRegion(Scene& scene, Bitmap& img, const float scale, int startX, int endX, int startY, int endY);

    static glm::vec3 clampColor(glm::vec3 color);
    glm::vec3 multyplyByLight(glm::vec3 color, glm::vec3 light);
    
    glm::vec3 getDiffuse(Hit& hit);
	
    glm::vec3 getBackgroundColor(glm::vec3& ray);

    glm::vec3 getOffset(Hit& surfaceHit, glm::vec3& direction);
    glm::vec3 castRay(glm::vec3& ray, glm::vec3 src, int reflects = 0);
    glm::vec3 castRayNoRecursion(glm::vec3& ray, glm::vec3 src, int reflects);
    Hit getHit(glm::vec3& ray, glm::vec3& src) const;
};