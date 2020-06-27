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

    void renderObject(std::vector<Object*>::value_type obj, Camera* camera) const
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

    void drawScene() const
    {
        auto camera = scene->getActiveCamera();
        if(camera == nullptr)
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
	void swapBuffers() const
	{
        glfwSwapBuffers(window->window);
    }
    virtual ~SceneRenderer() = default;
};
