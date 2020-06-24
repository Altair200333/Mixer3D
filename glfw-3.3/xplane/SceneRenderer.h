#pragma once
#include "mesh.h"
#include "scene.h"
#include "window.h"

class SceneRenderer
{
protected:
    const Window* window;
public:
	SceneRenderer(const Window* _window):window(_window){}
	
    void drawScene(Scene& scene) const
    {
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto obj : scene.objects)
        {
            obj->getComponent<MeshRenderer>()->render(scene.getActiveCamera());
        }

        glfwSwapBuffers(window->window);
    }
};
