#pragma once

#include <windows.h>
#include <shobjidl.h> 


#include "fileManager.h"
#include "gui.h"

#include "LightSource.h"
#include "Loger.h"
#include "ObjectLoader.h"
#include "SceneSaveLoader.h"

class MixerGUI: public GUI
{
	Scene* scene;

public:
	MixerGUI(Scene* _scene);
	void onStart(Window* window) override;
	void draw() override;

protected:
	void findAndDrawTransform(Object* obj);
	void drawColor(void* color);
	void drawMenuPanel();
	void drawLightPanel(Object* obj);
	void drawDragFloat(std::string label, float* val, float min, float max);
	void drawObjectPanel(Object* obj);
	void drawCameraPanel(Object* obj);
	void drawSceneSettings();
	void drawScenePanel();
	void drawLogsPanel();
	void drawControlsPanel();
};
