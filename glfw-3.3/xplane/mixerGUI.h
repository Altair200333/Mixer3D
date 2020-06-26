#pragma once

#include <windows.h>
#include <shobjidl.h> 

#include "gui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "LightSource.h"
#include "SceneSaveLoader.h"
class MixerGUI: public GUI
{
	Scene* scene;

public:
	MixerGUI(Scene* _scene ):scene(_scene){}
	void onStart(Window* window) override
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window->window, true);
		ImGui_ImplOpenGL3_Init((char*)glGetString(3));
	}

	void findAndDrawTransform(Object* obj)
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

	void drawColor(void* color)
	{
		ImGui::Text("color");
		ImGui::SameLine();
		ImGui::ColorEdit3("  ", (float*)(color));
	}
	
	void draw() override
	{
		
		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGui::Begin(std::string("Scene ["+scene->sceneName+"]").c_str(), nullptr, ImGuiWindowFlags_MenuBar);
		
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if(ImGui::MenuItem("Save"))
				{
					SceneSaveLoader::saveScene(*scene);
				}
				if (ImGui::MenuItem("Open"))
				{
					auto path = getPathDialog();
					std::cout << path << "l\n";
					if (!path.empty() && path.find(".mxr") != path.npos)
					{
						scene->clear();
						scene->sceneName = path;
						using json = nlohmann::json;

						std::ifstream i(path);
						json j;
						i >> j;

						SceneSaveLoader::fromJson(*scene, j);
					}
				}
				if (ImGui::MenuItem("Import"))
				{
					auto path = getPathDialog();
					std::cout << path << "i\n";
					if (!path.empty() && path.find(".stl")!=path.npos)
					{
						scene->AddObject(ObjectBuilder().addMesh(path).addRenderer(scene->window).addMaterial().addTransform());
					}
				}
				ImGui::EndMenu();
			}
			
			
			ImGui::EndMenuBar();
		}
		
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
						drawColor(&(mat->diffuseColor));
						ImGui::Text("roughness");
						ImGui::SameLine();
						ImGui::DragFloat("  ", (float*)&mat->roughness, 0.01, 0, 1);
						ImGui::PopID();
					}
					findAndDrawTransform(obj);
					ImGui::PushID(obj);
					if (ImGui::Button("Delete"))
					{
						auto it = std::find(scene->objects.begin(), scene->objects.end(), obj);
						scene->objects.erase(it);
						std::cout << "ss\n";
					}
					ImGui::PopID();

				}
			}
			ImGui::EndGroupPanel();
		}
		if (ImGui::CollapsingHeader("Lights"))
		{
			ImGui::BeginGroupPanel("list");
			for (auto obj : scene->lights)
			{
				if (ImGui::CollapsingHeader("Light"))
				{
					ImGui::PushID(obj);
					findAndDrawTransform(obj);
					ImGui::PopID();
					auto light = obj->getComponent<PointLight>();
					if (light != nullptr)
					{
						ImGui::PushID(light);
						drawColor(&light->color);
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
	void (*loadCallback)(std::string);
	std::string getPathDialog()
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							std::wstring ws(pszFilePath);
							std::string str(ws.begin(), ws.end());
							CoTaskMemFree(pszFilePath);
							return  str;
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		return "";
	}
};
