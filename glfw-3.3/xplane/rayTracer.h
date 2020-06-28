#pragma once
#include <corecrt_math_defines.h>
#include <thread>
#include <glm/glm/gtx/norm.hpp>


#include "bmpWriter.h"
#include "LightSource.h"
#include "mesh.h"
#include "renderEngine.h"
#include "vectorMath.h"


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
struct OptiMesh
{
    glm::vec3 pos;
    std::vector<PolygonMesh> polygons;
    Material* mat;
};
class RayTracerEngine : public RenderEngine
{
    int width, height;
    std::vector<OptiMesh> meshes;
    float maxPolygon = -1;
public:
    Bitmap* env;
	~RayTracerEngine() = default;

    void batchSceneMeshes(Scene& scene)
    {
	    for(auto obj:scene.objects)
	    {
		    OptiMesh om;
		    om.mat = obj->getComponent<Material>();
		    om.pos = obj->getComponent<Transform>()->position;
		    auto mesh = obj->getComponent<Mesh>();
		    for (int i = 0; i < mesh->vertexCount / 3; i++)
		    {
			    om.polygons.push_back(mesh->getPolygon(i));
                auto& pol = om.polygons[om.polygons.size() - 1];
                pol.vec1 += om.pos;
			    pol.vec2 += om.pos;
			    pol.vec3 += om.pos;
                maxPolygon = std::max<float>({maxPolygon, VectorMath::dist2(pol.vec1, pol.vec2), VectorMath::dist2(pol.vec1, pol.vec3), VectorMath::dist2(pol.vec2, pol.vec3) });
		    }
		    meshes.push_back(om);
	    }
	    Logger::log(std::to_string(meshes.size()));
	    Logger::log(std::to_string(maxPolygon));
	    std::cout << "finish\n";
    }

	Bitmap render(Scene& scene, int width, int height) override
	{
		Bitmap img;
        img.m_width = this->width = width;
        img.m_height = this->height = height;

		int l = img.m_width * img.m_height * 3;
		
		img.m_buffer = new uint8_t[l]();
		for (int i = 0; i < l; i++)
		{
			img.m_buffer[i] = 10;
		}
 
        env = scene.environment;
		batchSceneMeshes(scene);
		
        renderWithTracing(scene, img);
        return img;
	}
protected:

    void renderWithTracing(Scene& scene, Bitmap& img)
    {
        const float closeHeight = 2 * tan(scene.getActiveCamera()->Zoom / 2 * M_PI / 180);
        const float scale = closeHeight / height;

        const auto processor_count = std::thread::hardware_concurrency();

        Logger::log("Render started with " + std::to_string(processor_count)+" threads");
    	
        std::vector<std::thread> threads;
        const int subdivisions = processor_count;
    	
        for (int i = 0; i < subdivisions; ++i) {
	       
           int endX = width / subdivisions * (i + 1);
           int startX = width / subdivisions * i;
           
           threads.emplace_back([=, &scene, &img] {renderRegion(scene, img, scale, startX,
                endX,0, height); });
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
            	
                glm::vec3 c = castRay(ray, position,scene.lights, scene.maxBounces);

                const int id = img.getPixelId(i, height - j - 1, 0);

                img.m_buffer[id + 2] = c.r;
                img.m_buffer[id + 1] = c.g;
                img.m_buffer[id] = c.b;
            }
        }
    }

    static glm::vec3 clampColor(glm::vec3 color)
    {
        return { glm::clamp<int>(color.x, 10, 255),glm::clamp<int>(color.y, 10, 255),glm::clamp<int>(color.z, 10, 255) };
    }
	glm::vec3 getLight(glm::vec3 color, glm::vec3 light)
    {
        return { color.x * light.x, color.y * light.y ,color.z * light.z };
    }
    
    glm::vec3 getDiffuse(std::vector<Object*>& lights, Hit& hit)
    {
        glm::vec3 color = { 0, 0, 0 };
        for (auto light : lights)
        {
            glm::vec3 diffuse;
            glm::vec3 lightPosition = light->getComponent<Transform>()->position;
            glm::vec3 lightColor = light->getComponent<PointLight>()->color;

            glm::vec3 dirToLight = lightPosition - hit.pos;
            glm::vec3 offset = hit.pos + dirToLight * 0.0001f;
            Hit lhits = getHit(dirToLight, offset);

            if (!lhits.hit)
            {
                float slope = abs(glm::dot(hit.normal, glm::normalize(hit.pos - lightPosition)));

                diffuse = clampColor(getLight(hit.material->diffuseColor, lightColor) * 255.0f * slope);
            }
            else
            {
                diffuse = diffuse * 0.1f;
            }
            color += diffuse;
        }
        return color;
    }
	
    glm::vec2 getMapAngles(glm::vec3 ray) const
    {
        float theta = atan2(ray.z, ray.x) * 180 / M_PI + 180;
        float alpha = atan2(ray.y, sqrt(ray.x * ray.x + ray.z * ray.z)) * 180 / M_PI + 90;
        return { theta, alpha };
    }
	glm::vec3 getBackgroundColor(glm::vec3& ray)
    {
        glm::vec2 c = getMapAngles(ray);

        float x = env->m_width * c.x / 360;
        float y = env->m_height * c.y / 180;
        int id = env->getPixelId(env->m_width - x - 1, env->m_height - 1 - y, 0);
        return { env->m_buffer[id + 2],  env->m_buffer[id + 1], env->m_buffer[id] };
    }

    glm::vec3 getOffset(Hit& surfaceHit, glm::vec3& direction)
    {
	    return glm::dot(direction, surfaceHit.normal) < 0
		           ? surfaceHit.pos - surfaceHit.normal * 0.0001f
		           : surfaceHit.pos + surfaceHit.normal * 0.0001f;
    }
    glm::vec3 castRay(glm::vec3& ray, glm::vec3 src, std::vector<Object*>& lights, int reflects = 0)
    {
        glm::vec3 color = { 10, 10, 10 };
        Hit surfaceHit = getHit(ray, src);
        if (surfaceHit.hit)
        {
            color = getDiffuse(lights, surfaceHit);
            if (reflects > 0 && surfaceHit.material->roughness < 1)
            {
                glm::vec3 reflection = VectorMath::reflect(glm::normalize(ray), surfaceHit.normal);
                auto offset = getOffset(surfaceHit, reflection);
                glm::vec3 reflectedColor = castRay(reflection, offset, lights, reflects - 1);

            	
                glm::vec3 refraction = VectorMath::refract(glm::normalize(ray), surfaceHit.normal, surfaceHit.material->ior);
                offset = getOffset(surfaceHit, refraction);
                glm::vec3 refractedColor = castRay(refraction, offset, lights, reflects - 1);

                float nonTransparency = 1 - surfaceHit.material->transparency;
                float transparency = surfaceHit.material->transparency;
                float rough = surfaceHit.material->roughness;
                return clampColor((color * rough + reflectedColor * (1 - rough)) * nonTransparency + transparency * refractedColor);
            }
        }
        else
        {
            color = getBackgroundColor(ray);
        }
        return color;
    }
    
    Hit getHit(glm::vec3& ray, glm::vec3& src) const
    {
        Hit closestHit = { {100000, 100000, 100000}, {0, 0, 1}, {}, false };
        for (auto& m : meshes)
        {
            for (int i = 0; i < m.polygons.size(); i++)
            {
                glm::vec3 hit = VectorMath::intersectPoint(ray, src, m.polygons[i].normal, m.polygons[i].vec1);
                if(VectorMath::dist2(hit, m.polygons[i].vec1) > maxPolygon)
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
};