#include "rayTracer.h"

namespace 
{
    glm::vec2 getMapAngles(glm::vec3 ray)
    {
        float theta = atan2(ray.z, ray.x) * 180 / M_PI + 180;
        float alpha = atan2(ray.y, sqrt(ray.x * ray.x + ray.z * ray.z)) * 180 / M_PI + 90;
        return { theta, alpha };
    }
}
void RayTracingRenderer::batchSceneMeshes(Scene& scene)
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

void RayTracingRenderer::batchLight(Scene& scene)
{
	for (auto light : scene.lights)
	{
		lights.emplace_back(light->getComponent<Transform>()->position, light->getComponent<PointLight>());
	}
}

Bitmap RayTracingRenderer::render(Scene& scene, int width, int height)
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

    env = scene.environment;
    batchSceneMeshes(scene);
    batchLight(scene);
    renderWithTracing(scene, img);
    return img;
}

void RayTracingRenderer::renderWithTracing(Scene& scene, Bitmap& img)
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

void RayTracingRenderer::renderRegion(Scene& scene, Bitmap& img, const float scale, int startX, int endX, int startY, int endY)
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

glm::vec3 RayTracingRenderer::clampColor(glm::vec3 color)
{
    return { std::clamp<float>(color.x, 10, 255),std::clamp<float>(color.y, 10, 255),std::clamp<float>(color.z, 10, 255) };
}
glm::vec3 RayTracingRenderer::multyplyByLight(glm::vec3 color, glm::vec3 light)
{
    return { color.x * light.x, color.y * light.y ,color.z * light.z };
}

glm::vec3 RayTracingRenderer::getDiffuse(Hit& hit)
{
    glm::vec3 color = { 0, 0, 0 };
    for (auto light : lights)
    {
        glm::vec3 diffuse;
        glm::vec3 lightPosition = light.pos;
        glm::vec3 lightColor = light.light->color;


        glm::vec3 dirToLight = lightPosition - hit.pos;
        glm::vec3 offset = hit.pos + dirToLight * 0.0001f;
        Hit lightHits = getHit(dirToLight, offset);
        float attenuation = lightHits.hit ? lightHits.material->transparency : 1;
        while (lightHits.hit && lightHits.material->transparency != 0)
        {
            offset = lightHits.pos + dirToLight * 0.0001f;
            lightHits = getHit(dirToLight, offset);
            if (lightHits.hit)
                attenuation *= lightHits.material->transparency;
        }

        if (!lightHits.hit)
        {
            float slope = abs(glm::dot(hit.normal, glm::normalize(hit.pos - lightPosition)));

            diffuse = attenuation * clampColor(multyplyByLight(hit.material->diffuseColor, lightColor) * 255.0f * slope * (light.light->intensity / glm::length2(dirToLight)));
        }
        else
        {
            diffuse = diffuse * 0.1f;
        }
        color += diffuse;
    }
    return clampColor(color);
}


glm::vec3 RayTracingRenderer::getBackgroundColor(glm::vec3& ray)
{
    if (env == nullptr)
        return { 10,10,10 };
    glm::vec2 c = getMapAngles(ray);

    float x = env->m_width * c.x / 360;
    float y = env->m_height * c.y / 180;
    int id = env->getPixelId(env->m_width - x - 1, env->m_height - 1 - y, 0);
    return { env->m_buffer[id + 2],  env->m_buffer[id + 1], env->m_buffer[id] };
}

glm::vec3 RayTracingRenderer::getOffset(Hit& surfaceHit, glm::vec3& direction)
{
    return glm::dot(direction, surfaceHit.normal) < 0
        ? surfaceHit.pos - surfaceHit.normal * 0.0001f
        : surfaceHit.pos + surfaceHit.normal * 0.0001f;
}
glm::vec3 RayTracingRenderer::castRay(glm::vec3& ray, glm::vec3 src, int reflects)
{
    glm::vec3 color = { 10, 10, 10 };

    Hit surfaceHit = getHit(ray, src);
    if (surfaceHit.hit)
    {
        color = getDiffuse(surfaceHit);

        if (reflects > 0 && surfaceHit.material->roughness < 1)
        {
            glm::vec3 reflection = VectorMath::reflect(glm::normalize(ray), surfaceHit.normal);
            auto offset = getOffset(surfaceHit, reflection);
            glm::vec3 reflectedColor = castRay(reflection, offset, reflects - 1);


            glm::vec3 refraction = VectorMath::refract(glm::normalize(ray), surfaceHit.normal, surfaceHit.material->ior);
            offset = getOffset(surfaceHit, refraction);

            glm::vec3 refractedColor = (surfaceHit.material->transparency < 1) ? castRay(refraction, offset, reflects - 1) : glm::vec3(0, 0, 0);

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

Hit RayTracingRenderer::getHit(glm::vec3& ray, glm::vec3& src) const
{
    Hit closestHit = { {100000, 100000, 100000}, {0, 0, 1}, {}, false };
    for (auto& m : meshes)
    {
        for (int i = 0; i < m.polygons.size(); ++i)
        {
            glm::vec3 hit = VectorMath::intersectPoint(ray, src, m.polygons[i].normal, m.polygons[i].vec1);
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