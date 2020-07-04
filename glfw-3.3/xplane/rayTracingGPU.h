#pragma once
#include <corecrt_math_defines.h>
#include <thread>
#include <glm/glm/gtx/norm.hpp>
#include "renderEngine.h"
#include "MeshRenderer.h"
#include "mixerEngine.h"
#include "polygon.h"

class RayTracingGPURenderer final : public RenderEngine
{
    int width, height;
    struct Hit final
    {
        glm::vec3 pos;
        glm::vec3 normal;
        bool hit;

        Hit(glm::vec3 _pos, glm::vec3 _norm, bool _hit) : pos(_pos), normal(_norm), hit(_hit)
        {
        }
    };
    struct OptiPolygon final
    {
	    glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 v3;
        glm::vec3 normal;
        float maxd = -1;
        glm::vec3 color;
        float roughness;
        float transparency;
        float ior;
    };
    struct OptiLight
    {
	    glm::vec3 pos;
	    glm::vec3 color;
        float intensity;
    };
    std::vector<OptiPolygon> meshes;
    std::vector<float> polygons;
    std::vector<OptiLight> lights;
    
public:
    Shader shader;
	RayTracingGPURenderer()
	{
        shader = Shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs", "Shaders/geometry.gs");
	}
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
        batchSceneLights(scene);
        glMagic(scene);
        readPixels(img);
        //renderWithTracing(scene, img);
        return img;
    }

    void readPixels(Bitmap& img)
    {
	    //write data from hl buffer directly to image
	    glReadPixels(0, 0, img.m_width, img.m_height, GL_BGR, GL_UNSIGNED_BYTE, img.m_buffer);
		
		for(int i=0;i<img.m_height/2;i++)
		{
            for (int j = 0; j < img.m_width; j++)
            {
                int id1 = img.getPixelId(j, i, 0);
                int id2 = img.getPixelId(j, img.m_height-i-1, 0);
                std::swap(img.m_buffer[id1], img.m_buffer[id2]);
                std::swap(img.m_buffer[id1+1], img.m_buffer[id2+1]);
                std::swap(img.m_buffer[id1+2], img.m_buffer[id2+2]);
            }
		}
    }

	void glMagic(Scene& scene)
    {      
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
            shader.setVec3(prefix + "normal", meshes[i].normal);
            shader.setFloat(prefix + "maxd", meshes[i].maxd);
            shader.setVec3(prefix + "mat.color", meshes[i].color);
            shader.setFloat(prefix + "mat.roughness", meshes[i].roughness);
            shader.setFloat(prefix + "mat.transparency", meshes[i].transparency);
            shader.setFloat(prefix + "mat.ior", meshes[i].ior);
        }
        shader.setInt("polygonsCount", meshes.size());

        for (int i = 0; i < lights.size(); ++i)
        {
            std::string prefix = "lights[" + std::to_string(i) + "].";
            shader.setVec3(prefix + "color", lights[i].color);
            shader.setVec3(prefix + "pos", lights[i].pos);
            shader.setFloat(prefix + "intensity", lights[i].intensity);
        }
        shader.setInt("lightsCount", lights.size());

        float a[] = { 0.8, 0.8, 0.8 };
        unsigned int texture1;
        glGenBuffers(1, &texture1);
        glBindBuffer(GL_TEXTURE_BUFFER, texture1);
        glBufferData(GL_TEXTURE_BUFFER, 3 * sizeof(float), &a[0], GL_STATIC_DRAW);

        GLuint id;
        glGenTextures(1, &id);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, texture1);

        glBindBuffer(GL_TEXTURE_BUFFER, 0);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        //clear the screen
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // render the data
        glDrawArrays(GL_POINTS, 0, 1);
    	
    
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
        Hit surfaceHit = getHit(ray, src);
        if (surfaceHit.hit)
        {
            return glm::vec3(100, 100, 100);
        }
        return glm::vec3(10, 10, 10);
    }
    bool pointInPolygon(glm::vec3 v, OptiPolygon p)
    {
        float area = glm::length(glm::cross((p.v1 - p.v2), (p.v3 - p.v2))) * 0.5f;

        float a = glm::length(glm::cross((v - p.v2), (v - p.v3))) / (2 * area);
        float b = glm::length(glm::cross((v - p.v3), (v - p.v1))) / (2 * area);
        float c = glm::length(glm::cross((v - p.v2), (v - p.v1))) / (2 * area);

        float tresh = 1.0f + 0.0001f;
        return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
    }
    Hit getHit(glm::vec3& ray, glm::vec3& src) 
    {
        Hit closestHit = Hit( glm::vec3(100000, 100000, 100000), glm::vec3(0, 0, 1), false );

        for (int i = 0; i < meshes.size(); i++)
        {
	        glm::vec3 hit = VectorMath::intersectPoint(ray, src, meshes[i].normal, meshes[i].v1);
            if (VectorMath::dist2(hit, meshes[i].v1) > meshes[i].maxd)
                continue;
            if (pointInPolygon(hit, meshes[i])
                && VectorMath::dist2(closestHit.pos, src) > VectorMath::dist2(hit, src) && dot(hit - src, ray) > 0)
            {
                closestHit = Hit(hit, meshes[i].normal, true);
            }

        }

        return closestHit;
    }
protected:
    void batchSceneLights(Scene& scene)
    {
        for (auto obj : scene.lights)
        {
            OptiLight light;
            light.pos = obj->getComponent<Transform>()->position;
            light.color = obj->getComponent<PointLight>()->color;
        	light.intensity = obj->getComponent<PointLight>()->intensity;
            lights.push_back(light);
        }
    }
	void pushVector(glm::vec3& v)
    {
        polygons.push_back(v.x);
        polygons.push_back(v.y);
        polygons.push_back(v.z);
    }
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
            	om.roughness = obj->getComponent<Material>()->roughness;
            	om.transparency = obj->getComponent<Material>()->transparency;
            	om.ior = obj->getComponent<Material>()->ior;
                om.v1 = pol.vec1;
                om.v2 = pol.vec2;
                om.v3 = pol.vec3;
                om.normal = pol.normal;
                om.v1 += pos;
                om.v2 += pos;
                om.v3 += pos;
                om.maxd = std::max<float>({ om.maxd, VectorMath::dist2(om.v1, om.v2), VectorMath::dist2(om.v1, om.v3), VectorMath::dist2(om.v2, om.v3) });
                meshes.push_back(om);
            }
        }
    	for(auto& p:meshes)
    	{
            pushVector(p.v1);
            pushVector(p.v2);
            pushVector(p.v3);
            pushVector(p.normal);
            polygons.push_back(p.maxd);
            pushVector(p.color);
            polygons.push_back(p.roughness);
            polygons.push_back(p.transparency);
            polygons.push_back(p.ior);
    		
    	}
        Logger::log(std::to_string(meshes.size()));
    }
};
