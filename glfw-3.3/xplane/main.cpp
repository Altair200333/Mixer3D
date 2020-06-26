#include "mixerEngine.h"

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 900;

int main()
{
	MixerEngine engine(SCR_WIDTH, SCR_HEIGHT);


	std::ifstream i("conf.mxr");
	nlohmann::json j;
	i >> j;
	SceneSaveLoader::fromJson(engine.scene, j);
	engine.mainLoop();
	
	return 0;
}
