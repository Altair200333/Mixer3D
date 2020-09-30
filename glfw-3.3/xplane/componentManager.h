#pragma once
#include <map>
#include <string>

class Component;

class ComponentManager final
{
public:
	std::map<std::string, Component*> components;

	template <typename T>
	void addComponent(T* comp)
	{
		components.insert({typeid(T).name(), comp});
	}

	template <typename T>
	T* getComponent()
	{
		auto it = components.find(typeid(T).name());
		return it == components.end() ? nullptr : static_cast<T*>(it->second);
	}
	~ComponentManager();
};
