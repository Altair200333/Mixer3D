#pragma once
#include <iostream>
#include <utility>
#include "componentManager.h"

class Object final
{
	ComponentManager componentManager;
public:

	Object() = default;
	std::string name = "Object";
	Object(std::string _name) :name(_name){}
	
	template<typename T>
	Object* addComponent(T* comp)
	{
		if (comp == nullptr)
			return this;
		componentManager.addComponent<T>(comp);
		return this;
	}
	template<typename T>
	T* getComponent()
	{
		return componentManager.getComponent<T>();
	}
	~Object() = default;
};
