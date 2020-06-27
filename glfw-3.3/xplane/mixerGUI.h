#pragma once

#include <windows.h>
#include <shobjidl.h> 


#include "fileManager.h"
#include "gui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "LightSource.h"
#include "Loger.h"
#include "ObjectLoader.h"
#include "SceneSaveLoader.h"

class MixerGUI: public GUI
{
	Scene* scene;

public:
	MixerGUI(Scene* _scene ):scene(_scene){}
	void onStart(Window* window) override;
	void draw() override;

protected:
	void findAndDrawTransform(Object* obj);
	void drawColor(void* color);
	void drawMenuPanel();
	void drawLightPanel(std::vector<Object*>::value_type obj);
	void drawDragFloat(std::string label, float* val, float min, float max);
	void drawObjectPanel(Object* obj);
	void drawSceneSettings();
	void drawScenePanel();
	void drawLogsPanel();
};
