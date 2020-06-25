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
		
		ImGui::Begin("Objects");

		for(auto obj: scene->objects)
		{
			auto mat = obj->getComponent<Material>();
			
			if(mat!=nullptr)
			{
				ImGui::PushID(mat);
				ImGui::Text("Object: %s", obj->name);
				ImGui::ColorEdit3("color", (float*)&(mat->diffuseColor));
				ImGui::DragFloat("roughness", (float*)&mat->roughness, 0.01, 0, 1);
				ImGui::PopID();
				
			}
			auto transform = obj->getComponent<Transform>();
			if (transform != nullptr)
			{
				ImGui::PushID(transform);
				ImGui::DragFloat3("position", (float*)&transform->position, 0.01f);
				ImGui::PopID();
			}
			ImGui::NewLine();
		}
		ImGui::End();
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};
