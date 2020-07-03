#pragma once
#include <corecrt_math_defines.h>
#include <thread>
#include <glm/glm/gtx/norm.hpp>
#include "renderEngine.h"
#include "hit.h"
#include "MeshRenderer.h"
#include "mixerEngine.h"
#include "polygon.h"

class RayTracingGPURenderer final : public RenderEngine
{
    int width, height;
	
    struct OptiPolygon final
    {
	    glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 v3;
        float maxd = -1;
        glm::vec3 color;
    };
    std::vector<OptiPolygon> meshes;
    
public:
    Bitmap render(Scene& scene, int width, int height) override
    {
        Bitmap img;
        img.m_width = this->width = width;
        img.m_height = this->height = height;

        int l = img.m_width * img.m_height * 3;

        img.m_buffer = new uint8_t[l]();
        for (int i = 0; i < l; ++i)
        {
            img.m_buffer[i] = 10;
        }
        batchSceneMeshes(scene);

        glMagic(scene, img);
        //renderWithTracing(scene, img);
        return img;
    }
	void glMagic(Scene& scene, Bitmap& img)
    {      
        Shader shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs", "Shaders/geometry.gs");
        shader.use();

        const float closeHeight = 2 * tan(scene.getActiveCamera()->zoom / 2 * M_PI / 180);
        const float scale = closeHeight / height;
        auto camera = scene.getActiveCamera();
        shader.setVec3("camera.position", camera->owner->getComponent<Transform>()->position);
        shader.setFloat("camera.scale", scale);
        shader.setVec3("camera.front", camera->Front);
        shader.setVec3("camera.right", camera->Right);
        shader.setVec3("camera.up", camera->Up);
        shader.setInt("camera.width", width);
        shader.setInt("camera.height", height);

        for(int i=0; i<meshes.size(); ++i)
        {
            std::string prefix = "polygons[" + std::to_string(i) + "].";
            shader.setVec3(prefix + "v1", meshes[i].v1);
            shader.setVec3(prefix + "v2", meshes[i].v2);
            shader.setVec3(prefix + "v3", meshes[i].v3);
            shader.setFloat(prefix + "maxd", meshes[i].maxd);
            shader.setVec3(prefix + "color", meshes[i].color);
        }
        shader.setInt("polygonsCount", meshes.size());
        //clear the screen
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render the data
        glDrawArrays(GL_POINTS, 0, 1);
    	
        //write data from hl buffer directly to image
        glReadPixels(0, 0, img.m_width, img.m_height, GL_RGB, GL_UNSIGNED_BYTE, img.m_buffer);
    
    }
    void renderWithTracing(Scene& scene, Bitmap& img)
    {
        const float closeHeight = 2 * tan(scene.getActiveCamera()->zoom / 2 * M_PI / 180);
        const float scale = closeHeight / height;

        const auto processor_count = std::thread::hardware_concurrency();

        Logger::log("Render started with " + std::to_string(processor_count) + " threads");

        std::vector<std::thread> threads;
        const int subdivisions = processor_count;

        for (int i = 0; i < subdivisions; ++i) {

            int endX = width / subdivisions * (i + 1);
            int startX = width / subdivisions * i;

            threads.emplace_back([=, &scene, &img] {renderRegion(scene, img, scale, startX,
                endX, 0, height); });
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    void renderRegion(Scene& scene, Bitmap& img, const float scale, int startX, int endX, int startY, int endY)
    {
        Camera* camera = scene.getActiveCamera();
        glm::vec3 position = camera->owner->getComponent<Transform>()->position;

        for (int i = startX; i < endX; i += 1)
        {
            for (int j = startY; j < endY; j += 1)
            {
                glm::vec3 ray = camera->Front + camera->Right * float(i - width / 2) * scale +
                    camera->Up * float(j - height / 2) * scale;

                glm::vec3 color = castRay(ray, position, scene.maxBounces);

                const int id = img.getPixelId(i, height - j - 1, 0);

                img.m_buffer[id + 2] = color.r;
                img.m_buffer[id + 1] = color.g;
                img.m_buffer[id] = color.b;
            }
        }
    }
	
    glm::vec3 castRay(glm::vec3& ray, glm::vec3 src, int reflects)
    {
        
        return { 1,100,1 };
    }

protected:
   
    void batchSceneMeshes(Scene& scene)
    {
        for (auto obj : scene.objects)
        {
            glm::vec3 pos = obj->getComponent<Transform>()->position;
            auto mesh = obj->getComponent<Mesh>();
            for (int i = 0; i < mesh->vertexCount / 3; ++i)
            {
                PolygonMesh pol = mesh->getPolygon(i);
                OptiPolygon om;
                om.color = obj->getComponent<Material>()->diffuseColor;
                om.v1 = pol.vec1;
                om.v2 = pol.vec2;
                om.v2 = pol.vec2;
                om.v1 += pos;
                om.v2 += pos;
                om.v3 += pos;
                om.maxd = std::max<float>({ om.maxd, VectorMath::dist2(pol.vec1, pol.vec2), VectorMath::dist2(pol.vec1, pol.vec3), VectorMath::dist2(pol.vec2, pol.vec3) });
                meshes.push_back(om);
            }
        }
        Logger::log(std::to_string(meshes.size()));
    }
};
