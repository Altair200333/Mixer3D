#pragma once

#include "mesh.h"
#include "scene.h"
#include "window.h"


class SceneRenderer
{
protected:
    const Window* window;
    Scene* scene;
public:
   
    SceneRenderer(const Window* _window, Scene* _scene) :window(_window), scene(_scene)
    {    }
	void clearBuffer()
    {
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void drawScene() const
    {
        for (auto obj : scene->objects)
        {
            obj->getComponent<MeshRenderer>()->render(scene->getActiveCamera());
        }
        for (auto obj : scene->lights)
        {
            obj->getComponent<MeshRenderer>()->render(scene->getActiveCamera());
        }
        
    }
	void swapBuffers() const
	{
        glfwSwapBuffers(window->window);
    }
    virtual ~SceneRenderer() = default;
};
