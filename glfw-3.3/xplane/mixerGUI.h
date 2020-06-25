#pragma once
#include "gui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class MixerGUI: public GUI
{
	Scene* scene;
public:
	MixerGUI(Scene* _scene):scene(_scene){}
	void onStart(Window* window) override
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window->window, true);
		ImGui_ImplOpenGL3_Init((char*)glGetString(3));
	}
	void draw() override
	{
		
		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGui::Begin("OpenGL Texture Text");
		ImGui::Text("pointer = %p", 1);
		ImGui::End();
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};
