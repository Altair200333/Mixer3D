#pragma once
#include <cstdint>
#include "Object.h"
class Component
{
public:
	const static uint64_t componentID = 0;
	Object* owner;
	
	Component(Object* _owner):owner(_owner){}
	
};
template <typename T>
int getCompId()
{
	return T::componentID;
}