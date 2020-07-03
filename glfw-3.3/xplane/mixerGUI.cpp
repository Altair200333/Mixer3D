#include "mixerGUI.h"

#include "cameraBuilder.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

MixerGUI::MixerGUI(Scene* _scene): scene(_scene)
{
}

void MixerGUI::onStart(Window* window) 
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); io;
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
	drawControlsPanel();
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
	ImGui::Text("Color       ");
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
				scene->addObject(ObjectLoader::loadObject(path, scene->window));
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void MixerGUI::drawLightPanel(Object* obj)
{
	ImGui::PushID(obj);
	if (ImGui::CollapsingHeader("Light"))
	{
		findAndDrawTransform(obj);
		auto light = obj->getComponent<PointLight>();
		if (light != nullptr)
		{
			drawColor(&light->color);
			drawDragFloat("Intensity   ", &light->intensity, 0.1, 100);
		}
		ImGui::PushID(&obj->name);
		if (ImGui::Button("Delete"))
		{
			scene->deleteLight(obj);

			Logger::log("Delete light");
		}
		ImGui::PopID();
	}
	ImGui::PopID();
}
void MixerGUI::drawCameraPanel(Object* obj)
{
	ImGui::PushID(obj);
	if (ImGui::CollapsingHeader("Camera"))
	{
		findAndDrawTransform(obj);
		auto camera = obj->getComponent<Camera>();
		if (camera != nullptr)
		{
			drawDragFloat("fov", &camera->zoom, 5, 90);
		}
		ImGui::PushID(&camera->Front);
		if (ImGui::Button("Delete"))
		{
			scene->deleteCamera(obj);
			Logger::log("deleted camera ");
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::PushID(&obj->name);
		if (ImGui::Button("Set active"))
		{
			scene->setActiveCamera(obj);
			Logger::log("active camera updated");
		}
		ImGui::PopID();
	}
	ImGui::PopID();
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

				drawDragFloat("Roughness   ", &mat->roughness, 0, 1);
				drawDragFloat("Transparency", &mat->transparency, 0, 1);
				drawDragFloat("IOR         ", &mat->ior, 1, 3);


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
	ImGui::Text("Max bounces  ");
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
	const char* items[] = { "Ray Tracer", "Ray Marcher" };
	static int current_item = 1;

	if (ImGui::BeginCombo("##combo", items[current_item])) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = (current_item == n); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(items[n], is_selected))
				current_item = n;
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	scene->renderEngine = current_item;
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

		ImGui::PushID(&scene->lights);
		if (ImGui::Button("Add light"))
		{
			scene->addLight(LightBuilder().addLight({ 1,1,1 }, 1).addStandartSphere({1,1,1}, scene->window));
			Logger::log("Add light");
		}
		ImGui::PopID();
		ImGui::EndGroupPanel();
	}
	if (ImGui::CollapsingHeader("Cameras"))
	{
		ImGui::BeginGroupPanel("list");
		for (auto obj : scene->cameras)
		{
			drawCameraPanel(obj);
		}
		ImGui::PushID(&scene->cameras);
		if (ImGui::Button("Add camera"))
		{
			scene->addCamera(CameraBuilder().addCamera(static_cast<float>(scene->window->width) / scene->window->height, 60).addTransform({ 1,1,1 }));
			Logger::log("Add camera");
		}
		ImGui::PopID();
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

void MixerGUI::drawControlsPanel()
{
	ImGui::Begin("Controls");
	ImGui::TextColored(ImVec4(0.4, 0.5, 0.8, 1), "Move camera - WASD\\QE\nRender image - R");
	ImGui::End();
}
