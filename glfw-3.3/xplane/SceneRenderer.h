#pragma once

#include "scene.h"
#include "window.h"


class SceneRenderer final
{
protected:
    const Window* window;
    Scene* scene;
public:
   
    SceneRenderer(const Window* _window, Scene* _scene) :window(_window), scene(_scene)
    {    }
    void clearBuffer();

    void renderObject(std::vector<Object*>::value_type obj, Camera* camera) const;

    void drawScene() const;
    void swapBuffers() const;
    ~SceneRenderer() = default;
};
