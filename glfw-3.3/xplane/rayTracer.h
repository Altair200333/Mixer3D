#pragma once
#include <corecrt_math_defines.h>
#include <thread>



#include "bmpWriter.h"
#include "MeshRenderer.h"
#include "renderEngine.h"


class Hit
{
public:
    glm::vec3 pos;
    glm::vec3 normal;
    Material* material;
    bool hit;

    Hit(glm::vec3 _pos, glm::vec3 _norm, Material* _material, bool _hit) : pos(_pos), normal(_norm), material(_material),
        hit(_hit)
    {
    }
};
class Color
{
public:
    unsigned char r, g, b;

    Color(const unsigned char r_c, const unsigned char g_c, const unsigned char b_c) : r(r_c), g(g_c), b(b_c)
    {
    }
    Color() : r(20), g(20), b(20)
    {}
    friend Color operator+(const Color& c1, const Color& c2)
    {
        return { unsigned char((c1.r + c2.r) / 2),unsigned char((c1.g + c2.g) / 2),unsigned char((c1.b + c2.b) / 2) };
    }
    friend Color operator*(const Color& v1, const float f)
    {
        return { unsigned char(v1.r * f), unsigned char(v1.g * f), unsigned char(v1.b * f) };
    }
};
class RayTracerEngine : public RenderEngine
{
public:
    Bitmap* env;
	~RayTracerEngine() = default;
	Bitmap render(Scene& scene) override
	{
       
		Bitmap img;
		
		img.m_width = scene.width;
		img.m_height = scene.height;
		int l = img.m_width * img.m_height * 3;
		
		img.m_buffer = new uint8_t[l]();
		for (int i = 0; i < l; i++)
		{
			img.m_buffer[i] = 10;
		}
        BMPWriter bw;
        Bitmap envir = bw.loadJPG("env.jpg");
        env = &envir;
		
        renderTracing(scene, img);
        return img;
	}
protected:

    void renderTracing(Scene& scene, Bitmap& img)
    {
        const float closeH = 2 * tan(scene.getActiveCamera()->Zoom / 2 * M_PI / 180);
        const float scale = closeH / scene.height;

        const auto processor_count = std::thread::hardware_concurrency();
        std::cout << processor_count << "\n";
    	
        std::vector<std::thread> threads;
        const int divisions = processor_count;
    	
        for (int i = 0; i < divisions; ++i) {
	       
           int endX = scene.width / divisions * (i + 1);
           int startX = scene.width / divisions * i;
           
           threads.emplace_back([=, &scene, &img] {renderRegion(scene, img, scale, startX,
                endX,
                0, scene.height); });
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
                Camera* cam = scene.getActiveCamera();
                glm::vec3 ray = cam->Front + cam->Right * float(i - scene.width / 2) * scale +
                    cam->Up * float(j - scene.height / 2) * scale;
            	
                glm::vec3 c = castRay(scene.objects, ray, position, { 13, 13, 13 }, 1);

                const int id = img.getPixelId(i, scene.height - j - 1, 0);

                img.m_buffer[id + 2] = c.r;
                img.m_buffer[id + 1] = c.g;
                img.m_buffer[id] = c.b;
            }
        }
    }

    glm::vec3 reflect(const glm::vec3 I, const glm::vec3 N)
    {
        return I - N * (glm::dot(I, N) * 2.0f);
    }

    glm::vec3 getDiffuse(const std::vector<Object*>& meshes, glm::vec3 lp, Hit hit)
    {
        glm::vec3 diffuse = { 10, 10, 10 };
        glm::vec3 rr = lp - hit.pos;

        Hit lhits = getHit(meshes, rr, hit.pos + rr * 0.0001f);
    	
        float slope = abs(glm::dot(hit.normal, glm::normalize(hit.pos - lp)));

        diffuse.r = glm::clamp<int>(hit.material->diffuseColor.r * 255 * slope, 10, 255);
        diffuse.g = glm::clamp<int>(hit.material->diffuseColor.g * 255 * slope, 10, 255);
        diffuse.b = glm::clamp<int>(hit.material->diffuseColor.b * 255 * slope, 10, 255);

        if ((int(hit.pos.x * 10) + int(hit.pos.z * 10)) % 2 == 0)
            diffuse *= 0.1f;
    	
        if(lhits.hit)
        {
            diffuse = diffuse*0.1f;
        }
        return diffuse;
    }
    glm::vec2 getMapAngles(glm::vec3 ray)
    {
        float theta = atan2(ray.z, ray.x) * 180 / M_PI + 180;
        float alpha = atan2(ray.y, sqrt(ray.x * ray.x + ray.z * ray.z)) * 180 / M_PI + 90;
        return { theta, alpha };
    }
    glm::vec3 castRay(const std::vector<Object*>& objs, glm::vec3 ray, glm::vec3 src, glm::vec3 lp = { 13, 7, 13 }, int reflects = 0)
    {
        glm::vec3 diffuse = { 10, 10, 10 };
        Hit hit = getHit(objs, ray, src);
        if (hit.hit)
        {
            diffuse = getDiffuse(objs, lp, hit);
            if (reflects > 0 && hit.material->roughness < 1)
            {
                glm::vec3 r = reflect(glm::normalize(ray), hit.normal);
                glm::vec3 reflected = castRay(objs, r, hit.pos + hit.normal * 0.1f, lp, --reflects);
                return diffuse * (hit.material->roughness) + reflected * (1 - hit.material->roughness);
            }
        }
        else
        {
            glm::vec2 c = getMapAngles(ray);
            
            float x = env->m_width * c.x / 360;
            float y = env->m_height * c.y / 180;
            int id = env->getPixelId(env->m_width- x-1, env->m_height-1- y, 0);
            diffuse = { env->m_buffer[id+2],  env->m_buffer[id+1], env->m_buffer[id]};
        }
        return diffuse;
    }
    glm::vec3 intersectPoint(glm::vec3 rayVector, glm::vec3 rayPoint, glm::vec3 planeNormal, glm::vec3 planePoint) const
    {
        glm::vec3 diff = rayPoint - planePoint;
        double prod1 = glm::dot(diff, planeNormal);
        double prod2 = glm::dot(rayVector, planeNormal);
        float ratio = prod1 / prod2;

        return rayPoint - rayVector * ratio;
    }

    Hit getHit(const std::vector<Object*>& meshes, glm::vec3 ray, glm::vec3 src) const
    {
        Hit closestHit = { {100000, 100000, 100000}, {0, 0, 1}, {}, false };
        for (auto m : meshes)
        {
            Mesh* mesh = m->getComponent<Mesh>();
            glm::vec3 pos = m->getComponent<Transform>()->position;
            for (int i = 0; i < mesh->vertexCount / 3; i++)
            {

                glm::vec3 hit = intersectPoint(ray, src,
                    mesh->getNormal(i * 3),
                    mesh->getVertex(i * 3)+pos);
                if (glm::length2(hit - mesh->getVertex(i * 3)-pos) > 8)
                    continue;
                if (Mesh::pointInPolygon(hit, mesh->getVertex(i * 3)+pos,
                    mesh->getVertex(i * 3 + 1)+pos,
                    mesh->getVertex(i * 3 + 2)+pos) && glm::length2(closestHit.pos - src) > glm::length2(hit - src) && glm::dot(hit - src, ray) > 0)
                {
                    closestHit = { hit, mesh->getNormal(i * 3), m->getComponent<Material>(), true };
                }
            }
        }

        return closestHit;
    }
};