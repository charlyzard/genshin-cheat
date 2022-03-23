#pragma once
#include <string>
#include <vector>

#include <gcclib/Logger.h>

#include "field/ToggleField.h"
#include "field/BaseField.h"

#define NFF(field, ...) field##(config::NewField<decltype(field)>(__VA_ARGS__))
#define NF(field, name, section, ...) NFF(field, name, #field, section, __VA_ARGS__)

namespace config 
{
	void Init(const std::string configFile);
	void Save();

	std::vector<ConfigEntry*> GetFields();
	std::vector<field::ToggleField*> GetToggleFields();

	void AddField(ConfigEntry& entry);
	void AddField(field::ToggleField& entry);

	template<class R, typename ... Args>
	R NewField(Args ... args) 
	{
		auto value = R(args ...);
		AddField(value);
		return value;
	}
}

