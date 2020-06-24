#pragma once
#include <map>

#include "component.h"
class Component;

class ComponentManager final
{
public:
	std::map<uint64_t, Component*> components;

	template <typename T>
	void addComponent(T* comp)
	{
		components.insert({T::componentID, comp});
	}

	template <typename T>
	T* getComponent()
	{
		auto it = components.find(T::componentID);
		return it == components.end() ? nullptr : static_cast<T*>(it->second);
	}
	~ComponentManager()
	{
		for(auto comp:components)
		{
			free(comp.second);
		}
	}
};
