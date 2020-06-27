#include "mixerGUI.h"


namespace 
{
	
}
void MixerGUI::onStart(Window* window) 
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window->window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(3));
}
void MixerGUI::draw()
{

	ImGui_ImplOpenGL3_NewFrame();

	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	drawScenePanel();
	drawLogsPanel();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void MixerGUI::findAndDrawTransform(Object* obj)
{
	auto transform = obj->getComponent<Transform>();
	if (transform != nullptr)
	{
		ImGui::PushID(transform);
		ImGui::Text("position    ");
		ImGui::SameLine();
		ImGui::DragFloat3("  ", (float*)&transform->position, 0.01f);
		ImGui::PopID();
	}
}

void MixerGUI::drawColor(void* color)
{
	ImGui::PushID(color);
	ImGui::Text("color       ");
	ImGui::SameLine();
	ImGui::ColorEdit3("  ", (float*)(color));
	ImGui::PopID();
}

void MixerGUI::drawMenuPanel()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				SceneSaveLoader::saveScene(*scene);
				Logger::log("Scene saved");
			}
			if (ImGui::MenuItem("Open"))
			{
				auto path = FileManager::getPathDialog();
				SceneSaveLoader::loadScene(*scene, path);
			}
			if (ImGui::MenuItem("Import"))
			{
				auto path = FileManager::getPathDialog();
				scene->AddObject(ObjectLoader::loadObject(path, scene->window));
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void MixerGUI::drawLightPanel(std::vector<Object*>::value_type obj)
{
	if (ImGui::CollapsingHeader("Light"))
	{
		ImGui::PushID(obj);
		findAndDrawTransform(obj);
		ImGui::PopID();
		auto light = obj->getComponent<PointLight>();
		if (light != nullptr)
		{
			drawColor(&light->color);
		}
	}
}

void MixerGUI::drawDragFloat(std::string label, float* val, float min, float max)
{
	ImGui::PushID(val);
	ImGui::Text(label.c_str());
	ImGui::SameLine();
	ImGui::SliderFloat("", val, min, max);
	ImGui::PopID();
}

void MixerGUI::drawObjectPanel(Object* obj)
{
	ImGui::PushID(obj);
	if (ImGui::CollapsingHeader(std::string("Object: " + obj->name).c_str()))
	{
		auto mat = obj->getComponent<Material>();
		if (mat != nullptr)
		{
			ImGui::PushID(mat);
			if (ImGui::CollapsingHeader(std::string("material").c_str()))
			{
				drawColor(&(mat->diffuseColor));

				drawDragFloat("roughness   ", &mat->roughness, 0, 1);
				drawDragFloat("transparency", &mat->transparency, 0, 1);
				drawDragFloat("ior         ", &mat->ior, 1, 3);


			}
			ImGui::PopID();
		}
		findAndDrawTransform(obj);
		ImGui::PushID(&obj->name);
		if (ImGui::Button("Delete"))
		{
			scene->deleteObject(obj);

			Logger::log("Delete object");
		}
		ImGui::PopID();
	}
	ImGui::PopID();
}

void MixerGUI::drawSceneSettings()
{
	ImGui::PushID(&scene->maxBounces);
	ImGui::Text("Max bounces");
	ImGui::SameLine();
	ImGui::SliderInt("", &scene->maxBounces, 0, 64);
	ImGui::Text(scene->envPath.empty() ? "Not specified" : scene->envPath.c_str());
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::PushID(&FileManager::getPathDialog);
	if (ImGui::Button("load"))
	{
		scene->loadEnvironment(FileManager::getPathDialog());

	}
	ImGui::PopID();
}

void MixerGUI::drawScenePanel()
{
	ImGui::Begin(std::string("Scene [" + scene->sceneName + "]").c_str(), nullptr, ImGuiWindowFlags_MenuBar);

	drawMenuPanel();
	drawSceneSettings();

	if (ImGui::CollapsingHeader("Objects"))
	{
		ImGui::BeginGroupPanel("list");
		for (auto obj : scene->objects)
		{
			drawObjectPanel(obj);
		}

		ImGui::EndGroupPanel();
	}
	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::BeginGroupPanel("list");
		for (auto obj : scene->lights)
		{
			drawLightPanel(obj);
		}
		ImGui::EndGroupPanel();
	}
	ImGui::End();
}

void MixerGUI::drawLogsPanel()
{
	ImGui::Begin("Logs");
	ImGui::TextColored(ImVec4(0.2, 1, 0.2, 1), Logger::getReport().c_str());
	ImGui::End();
}
