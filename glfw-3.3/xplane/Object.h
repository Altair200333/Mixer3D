#pragma once
#include <utility>
#include "componentManager.h"

class Object
{
	ComponentManager componentManager;
public:

	Object() = default;


	template<typename T>
	Object* addComponent(T* comp)
	{
		componentManager.addComponent<T>(comp);
		return this;
	}
	template<typename T>
	T* getComponent()
	{
		return componentManager.getComponent<T>();
	}	
};
