#include "SceneRenderer.h"

#include "MeshRenderer.h"


void SceneRenderer::clearBuffer()
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneRenderer::renderObject(std::vector<Object*>::value_type obj, Camera* camera) const
{
    auto meshRenderer = obj->getComponent<MeshRenderer>();
    if (meshRenderer != nullptr)
    {
        meshRenderer->render(camera);
    }
    else
    {
        Logger::log("Mesh renderer is not set");
    }
}

void SceneRenderer::drawScene() const
{
    auto camera = scene->getActiveCamera();
    if (camera == nullptr)
    {
        Logger::log("Camera not set");
        return;
    }
    for (auto obj : scene->objects)
    {
        renderObject(obj, camera);
    }
    for (auto obj : scene->lights)
    {
        renderObject(obj, camera);
    }

}
void SceneRenderer::swapBuffers() const
{
    window->swapBuffers();
}