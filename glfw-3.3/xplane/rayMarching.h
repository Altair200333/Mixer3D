#pragma once
#include <corecrt_math_defines.h>
#include <thread>
#include <glm/glm/gtx/norm.hpp>
#include "renderEngine.h"
#include "hit.h"
#include "MeshRenderer.h"

class RayMarchingRenderer final : public RenderEngine
{
    int width, height;
    struct OptiMesh final
    {
        glm::vec3 pos;
        std::vector<PolygonMesh> polygons;
        Material* mat;
    };
    std::vector<OptiMesh> meshes;
    
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
        Shader shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs");
        shader.use();
        shader.setVec3("viewPos", scene.getActiveCamera()->owner->getComponent<Transform>()->position);

        // material properties
        shader.setVec3("diffuse", scene.objects[0]->getComponent<Material>()->diffuseColor);

        // view/projection transformations
        const glm::mat4 projection = glm::perspective(glm::radians(scene.getActiveCamera()->zoom), scene.getActiveCamera()->aspectRatio, 0.1f, 1000.0f);
        const glm::mat4 view = scene.getActiveCamera()->GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // world transformation
        const glm::mat4 worldModel = glm::translate(scene.objects[0]->getComponent<MeshRenderer>()->model, scene.objects[0]->getComponent<Transform>()->position);
        shader.setMat4("model", worldModel);

        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render the cube
        glDrawArrays(GL_TRIANGLES, 0, 0);

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
	float getMinDst(glm::vec3 src)
    {
        const glm::vec3 front = { 0,0,1 };
        const glm::vec3 up = { 0,1,0 };
        const glm::vec3 right = { 1,0,0 };
        float step = 0.1f;
        float dst = 100000;
	    for(float i=0;i<2*M_PI;i+=step)
	    {
	    	
            for (float j = 0; j < M_PI; j+=step)
            {

                float x = sin(j) * cos(i);
                float y = sin(j) * sin(i);
                float z = cos(j);
                glm::vec3 ray = right * x + front * y + up * z;
                Hit hit = getHit(ray, src);
            	if(hit.hit)
					dst = min(dst, glm::length2(hit.pos - src));
            }
	    }
        return dst;
    }
    glm::vec3 castRay(glm::vec3& ray, glm::vec3 src, int reflects)
    {
        float minDst = 90000;
    	
        float dst = getMinDst(src);
        
        return { 1,dst>=minDst?1:100,1 };
    }

protected:
    Hit getHit(glm::vec3& ray, glm::vec3& src) const
    {
    	//just make real remote hit that doesn't hit anything
        Hit closestHit = { {100000, 100000, 100000}, {0, 0, 1}, {}, false };
        for (auto& m : meshes)
        {
            for (int i = 0; i < m.polygons.size(); ++i)
            {
                glm::vec3 hit = VectorMath::intersectPoint(ray, src, m.polygons[i].normal, m.polygons[i].vec1);
            	//cheap check for point being in radius of polygon
                if (VectorMath::dist2(hit, m.polygons[i].vec1) > m.polygons[i].maxDistance)
                    continue;
                if (VectorMath::pointInPolygon(hit, m.polygons[i])
                    && VectorMath::dist2(closestHit.pos, src) > VectorMath::dist2(hit, src) && glm::dot(hit - src, ray) > 0)
                {
                    closestHit = { hit, m.polygons[i].normal, m.mat, true };
                }
            }
        }

        return closestHit;
    }
    void batchSceneMeshes(Scene& scene)
    {
        for (auto obj : scene.objects)
        {
            OptiMesh om;
            om.mat = obj->getComponent<Material>();
            om.pos = obj->getComponent<Transform>()->position;
            auto mesh = obj->getComponent<Mesh>();
            for (int i = 0; i < mesh->vertexCount / 3; ++i)
            {
                om.polygons.push_back(mesh->getPolygon(i));
                auto& pol = om.polygons[om.polygons.size() - 1];
                pol.vec1 += om.pos;
                pol.vec2 += om.pos;
                pol.vec3 += om.pos;
                pol.maxDistance = std::max<float>({ pol.maxDistance, VectorMath::dist2(pol.vec1, pol.vec2), VectorMath::dist2(pol.vec1, pol.vec3), VectorMath::dist2(pol.vec2, pol.vec3) });
            }
            meshes.push_back(om);
        }
        Logger::log(std::to_string(meshes.size()));
    }
};
