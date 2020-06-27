#include "componentManager.h"
#include "component.h"

ComponentManager::~ComponentManager()
{
	for (auto comp : components)
	{
		delete comp.second;
	}
}
