#pragma once
#include <cstdint>
#include "Object.h"

class Object;
class Component
{
public:
	Object* owner;
	
	Component(Object* _owner):owner(_owner){}
		
	virtual ~Component() = default;
};
template <typename T>
int getCompId()
{
	return T::componentID;
}