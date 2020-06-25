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

	void drawTransform(Object* obj)
	{
		auto transform = obj->getComponent<Transform>();
		if (transform != nullptr)
		{
			ImGui::PushID(transform);
			ImGui::Text("position");
			ImGui::SameLine();
			ImGui::DragFloat3("  ", (float*)&transform->position, 0.01f);
			ImGui::PopID();
		}
	}

	void draw() override
	{
		
		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGui::Begin("Scene");
		if (ImGui::CollapsingHeader("Objects"))
		{
			ImGui::BeginGroupPanel("list");
			for (auto obj : scene->objects)
			{
				
				if (ImGui::CollapsingHeader(std::string("Object: " + obj->name).c_str()))
				{
					auto mat = obj->getComponent<Material>();
					if (mat != nullptr)
					{
						ImGui::PushID(mat);
						ImGui::Text("color");
						ImGui::SameLine();
						ImGui::ColorEdit3("  ", (float*)&(mat->diffuseColor));
						ImGui::Text("roughness");
						ImGui::SameLine();
						ImGui::DragFloat("  ", (float*)&mat->roughness, 0.01, 0, 1);
						ImGui::PopID();
					}
					drawTransform(obj);					
				}
			}
			ImGui::EndGroupPanel();
		}
		if (ImGui::CollapsingHeader("Lights"))
		{
			ImGui::BeginGroupPanel("list");
			for (auto obj : scene->lights)
			{
				ImGui::PushID(obj);
				if (ImGui::CollapsingHeader("Light"))
				{
					ImGui::PopID();
					drawTransform(obj);
					auto light = obj->getComponent<PointLight>();
					if (light != nullptr)
					{
						ImGui::PushID(light);
						ImGui::Text("color");
						ImGui::SameLine();
						ImGui::ColorEdit3("  ", (float*)&(light->color));
						ImGui::PopID();
					}
				}
			}
			ImGui::EndGroupPanel();
		}
		ImGui::End();
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
protected:
	
};
