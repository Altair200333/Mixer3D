#include "mixerEngine.h"

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 900;

int main()
{
	MixerEngine engine(SCR_WIDTH, SCR_HEIGHT);
	engine.scene.loadSceneFromJson("conf.mxr");
	engine.mainLoop();
	
	return 0;
}
