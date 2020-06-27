#pragma once
#include <glm/glm/vec3.hpp>
#include <nlohmann/json.hpp>
#include <string>

class ItemBuilder
{
public:
	virtual ~ItemBuilder() = default;
protected:

	static float getFloatOr(nlohmann::json& j, std::string key, float def)
	{
		if (j.find(key) != j.end())
		{
			return j.at(key);
		}
		else
			return def;
	}

	static glm::vec3 getVec3Or(nlohmann::json& j, std::string key, glm::vec3 def)
	{
		if (j.find(key) != j.end() && j.at(key).size() == 3)
		{
			return { j.at(key)[0],j.at(key)[1],j.at(key)[2] };
		}
		else
			return def;
	}
};
