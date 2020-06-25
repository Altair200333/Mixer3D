#pragma once
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "mesh.h"
#include "scene.h"
#include "window.h"


class SceneRenderer
{
protected:
    const Window* window;
public:
    SceneRenderer(const Window* _window) :window(_window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window->window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(3));
    }
	
    void drawScene(Scene& scene) const
    {
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto obj : scene.objects)
        {
            obj->getComponent<MeshRenderer>()->render(scene.getActiveCamera());
        }
        for (auto obj : scene.lights)
        {
            obj->getComponent<MeshRenderer>()->render(scene.getActiveCamera());
        }
        ImGui_ImplOpenGL3_NewFrame();

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window->window);
    }
};
