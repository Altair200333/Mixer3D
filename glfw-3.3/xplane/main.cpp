#include "mixerEngine.h"

// settings
const unsigned int SCR_WIDTH = 1820;
const unsigned int SCR_HEIGHT = 900;

int main()
{
	MixerEngine engine(SCR_WIDTH, SCR_HEIGHT);
	SceneSaveLoader::loadScene(engine.scene, "conf.mxr");
	engine.mainLoop();
	
	return 0;
}
