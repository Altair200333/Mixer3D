#pragma once
#include <cstdint>
#include "Object.h"

class Object;
class Component
{
public:
	const static uint64_t componentID = 0;
	Object* owner;
	
	Component(Object* _owner):owner(_owner){}
		
	virtual ~Component() = default;
};
template <typename T>
int getCompId()
{
	return T::componentID;
}