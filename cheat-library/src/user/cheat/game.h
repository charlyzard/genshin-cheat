#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include <il2cpp-appdata.h>

namespace cheat::game
{
	template<class type>
	struct EntityFilterField
	{
		bool enabled;
		type value;
	};

	struct WaypointInfo
	{
		uint32_t sceneId = 0;
		uint32_t waypointId = 0;
		app::Vector3 position = {};
		app::MapModule_ScenePointData* data = nullptr;
	};

	struct SimpleEntityFilter
	{
		EntityFilterField<app::EntityType__Enum_1> typeFilter;
		EntityFilterField<std::vector<std::string>> nameFilter;
	};

	// Position utils
	app::Vector3 GetRelativePosition(app::BaseEntity* entity);
	void SetRelativePosition(app::BaseEntity* entity, app::Vector3 position);
	app::Vector3 GetAvatarRelativePosition();

	app::Vector3 GetAbsolutePosition(app::BaseEntity* entity);
	void SetAbsolutePosition(app::BaseEntity* entity, app::Vector3 position);
	app::Vector3 GetAvatarAbsolutePosition();

	app::BaseEntity* GetAvatarEntity();
	app::CameraEntity* GetMainCameraEntity();
	void SetAvatarRelativePosition(app::Vector3 position);
	float GetDistToAvatar(app::BaseEntity* entity);

	std::vector<app::BaseEntity*> GetEntities();
	app::BaseEntity* GetEntityByRuntimeId(uint32_t runtimeId);
	uint32_t GetAvatarRuntimeId();

	// Entity filters
	typedef bool (*FilterFunc)(app::BaseEntity* entity);

	std::vector<app::BaseEntity*> FindEntities(FilterFunc func);
	std::vector<app::BaseEntity*> FindEntities(const SimpleEntityFilter& filter);

	app::BaseEntity* FindNearestEntity(FilterFunc func);
	app::BaseEntity* FindNearestEntity(const SimpleEntityFilter& filter);
	app::BaseEntity* FindNearestEntity(std::vector<app::BaseEntity*>& entities);

	bool IsEntityFilterValid(app::BaseEntity* entity, const SimpleEntityFilter& filter);

	const SimpleEntityFilter& GetFilterCrystalShell();
	const SimpleEntityFilter& GetFilterChest();
	const SimpleEntityFilter& GetMonsterFilter();

	bool IsEntityCrystalShell(app::BaseEntity* entity);

	WaypointInfo FindNearestWaypoint(app::Vector3& position);
	static std::vector<WaypointInfo> GetUnlockedWaypoints();
}
