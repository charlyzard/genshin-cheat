#include <pch-il2cpp.h>
#include "game.h"

#include <chrono>

#include <Windows.h>
#include <helpers.h>

namespace cheat::game 
{
	app::Vector3 GetRelativePosition(app::BaseEntity* entity)
	{
		if (entity == nullptr)
			return {};

		return app::BaseEntity_GetRelativePosition(entity, nullptr);
	}

	void SetRelativePosition(app::BaseEntity* entity, app::Vector3 position)
	{
		if (entity == nullptr)
			return;

		app::BaseEntity_SetRelativePosition(entity, position, true, nullptr);
	}

	app::Vector3 GetAbsolutePosition(app::BaseEntity* entity)
	{
		if (entity == nullptr)
			return {};

		return app::BaseEntity_GetAbsolutePosition(entity, nullptr);
	}

	void SetAbsolutePosition(app::BaseEntity* entity, app::Vector3 position)
	{
		if (entity == nullptr)
			return;

		app::BaseEntity_SetAbsolutePosition(entity, position, true, nullptr);
	}

	app::GadgetEntity* GetGadget(uint32_t runtimeID)
	{
		return GetGadget(GetEntityByRuntimeId(runtimeID));
	}

	app::GadgetEntity* GetGadget(app::BaseEntity* entity)
	{
		if (entity == nullptr)
			return nullptr;

		if (!IsEntityGadget(entity))
			return nullptr;
		
		return reinterpret_cast<app::GadgetEntity*>(entity);
	}

	bool IsEntityGadget(app::BaseEntity* entity)
	{
		return entity->fields.entityType == app::EntityType__Enum_1::Gadget ||
			entity->fields.entityType == app::EntityType__Enum_1::Bullet ||
			entity->fields.entityType == app::EntityType__Enum_1::Field;
	}

	app::BaseEntity* GetAvatarEntity()
	{
		auto entityManager = GetSingleton(EntityManager);
		if (entityManager == nullptr)
			return nullptr;

		auto avatarEntity = app::EntityManager_GetCurrentAvatar(entityManager, nullptr);
		return avatarEntity;
	}

	app::CameraEntity* GetMainCameraEntity()
	{
		auto entityManager = GetSingleton(EntityManager);
		if (entityManager == nullptr)
			return nullptr;

		auto cameraEntity = app::EntityManager_GetMainCameraEntity(entityManager, nullptr);
		return cameraEntity;
	}

	app::Vector3 GetAvatarAbsolutePosition()
	{
		return app::BaseEntity_GetAbsolutePosition(GetAvatarEntity(), nullptr);
	}

	app::Vector3 GetAvatarRelativePosition()
	{
		return app::BaseEntity_GetRelativePosition(GetAvatarEntity(), nullptr);
	}

	void SetAvatarRelativePosition(app::Vector3 position)
	{
		SetRelativePosition(GetAvatarEntity(), position);
	}

	float GetDistToAvatar(app::BaseEntity* entity)
	{
		auto dist = app::Vector3_Distance(nullptr, GetAvatarRelativePosition(), GetRelativePosition(entity), nullptr);
		return dist;
	}

	bool IsEntityFilterValid(app::BaseEntity* entity, const SimpleEntityFilter& filter)
	{
		if (entity == nullptr)
			return false;

		if (filter.typeFilter.enabled && entity->fields.entityType != filter.typeFilter.value)
			return false;

		if (filter.nameFilter.enabled)
		{
			bool found = false;
			auto name = il2cppi_to_string(app::BaseEntity_ToStringRelease(entity, nullptr));
			for (auto& pattern : filter.nameFilter.value)
			{
				if (name.find(pattern) != -1) {
					found = true;
					break;
				}
			}
			if (!found)
				return false;
		}
		return true;
	}

	std::vector<app::BaseEntity*> GetEntities()
	{
		auto entityManager = GetSingleton(EntityManager);
		if (entityManager == nullptr)
			return {};

		auto entities = ToUniList(app::EntityManager_GetEntities(entityManager, nullptr), app::BaseEntity*);
		if (entities == nullptr)
			return {};

		std::vector<app::BaseEntity*> aliveEntities;
		aliveEntities.reserve(entities->size);

		for (const auto& entity : *entities)
		{
			if (entity != nullptr)
				aliveEntities.push_back(entity);
		}
		return aliveEntities;
	}

	uint32_t GetAvatarRuntimeId()
	{
		auto entityManager = GetSingleton(EntityManager);
		if (entityManager == nullptr)
			return 0;

		auto avatarEntity = GetAvatarEntity();
		if (avatarEntity == nullptr)
			return 0;

		return avatarEntity->fields._runtimeID_k__BackingField;
	}

	bool IsAvatarEntity(app::BaseEntity* entity)
	{
		if (entity == nullptr)
			return false;

		auto avatarID = GetAvatarRuntimeId();
		if (avatarID == 0)
			return false;

		return avatarID == entity->fields._runtimeID_k__BackingField;
	}

	app::BaseEntity* GetEntityByRuntimeId(uint32_t runtimeId)
	{
		auto entityManager = GetSingleton(EntityManager);
		if (entityManager == nullptr)
			return nullptr;

		return app::EntityManager_GetValidEntity(entityManager, runtimeId, nullptr);
	}

	std::vector<app::BaseEntity*> FindEntities(const SimpleEntityFilter& filter)
	{
		std::vector<app::BaseEntity*> result{};
		for (auto& entity : GetEntities())
		{
			if (IsEntityFilterValid(entity, filter))
				result.push_back(entity);
		}

		return result;
	}

	std::vector<app::BaseEntity*> FindEntities(FilterFunc func)
	{
		std::vector<app::BaseEntity*> result{};
		for (auto& entity : GetEntities())
		{
			if (func(entity))
				result.push_back(entity);
		}
		return result;
	}

	app::BaseEntity* FindNearestEntity(FilterFunc func)
	{
		auto entities = FindEntities(func);
		return FindNearestEntity(entities);
	}

	app::BaseEntity* FindNearestEntity(const SimpleEntityFilter& filter)
	{
		auto entities = FindEntities(filter);
		return FindNearestEntity(entities);
	}

	app::BaseEntity* FindNearestEntity(std::vector<app::BaseEntity*>& entities)
	{
		if (entities.size() == 0)
			return nullptr;

		return *std::min_element(entities.begin(), entities.end(),
			[](app::BaseEntity* a, app::BaseEntity* b) { return GetDistToAvatar(a) < GetDistToAvatar(b); });
	}

	const SimpleEntityFilter& GetFilterCrystalShell()
	{
		static const SimpleEntityFilter crystallShellFilter = {
			{true, app::EntityType__Enum_1::GatherObject},
			{true, {
				/*Anemoculus, Geoculus*/ "CrystalShell" ,
				/*Crimson Agate*/ "Prop_Essence_01",
				/*Electroculus*/ "ElectricCrystal",
				/*Lumenspar*/ "CelestiaSplinter_01",
				/*Key Sigil*/ "RuneContent_04"
				}
			}
		};
		return crystallShellFilter;
	}

	const SimpleEntityFilter& GetFilterChest()
	{
		static const SimpleEntityFilter filter = {
			{true, app::EntityType__Enum_1::Chest}
		};
		return filter;
	}

	const SimpleEntityFilter& GetMonsterFilter()
	{
		static const SimpleEntityFilter filter = {
			{true, app::EntityType__Enum_1::Monster}
		};
		return filter;
	}

	bool IsEntityCrystalShell(app::BaseEntity* entity) {

		return IsEntityFilterValid(entity, GetFilterCrystalShell());
	}

	std::vector<WaypointInfo> GetUnlockedWaypoints(uint32_t targetSceneId)
	{
		auto mapModule = GetSingleton(MBHLOBDPKEC);
		if (mapModule == nullptr)
			return {};

		if (targetSceneId == 0)
			targetSceneId = GetCurrentPlayerSceneID();

		auto result = std::vector<WaypointInfo>();

		auto waypointGroups = ToUniDict(mapModule->fields._scenePointDics, uint32_t, UniDict<uint32_t COMMA app::MapModule_ScenePointData>*);
		for (const auto& [sceneId, waypoints] : waypointGroups->pairs())
		{
			if (sceneId != targetSceneId)
				continue;
			
			for (const auto& [waypointId, waypoint] : waypoints->pairs())
			{
				auto &config = waypoint.config->fields;
				uint16_t areaId = app::SimpleSafeUInt16_get_Value(nullptr, config.areaIdRawNum, nullptr);
				bool isAreaUnlocked = app::MapModule_IsAreaUnlock(mapModule, sceneId, areaId, nullptr);

				if (waypoint.isUnlocked && isAreaUnlocked && !config._unlocked && !waypoint.isGroupLimit && !waypoint.isModelHidden)
					result.push_back(WaypointInfo{ sceneId, waypointId, waypoint.config->fields._tranPos, (app::MapModule_ScenePointData*)&waypoint });
			}
		}
		return result;
	}

	// Finding nearest unlocked waypoint to the position
	WaypointInfo FindNearestWaypoint(app::Vector3& position, uint32_t targetSceneId)
	{
		float minDistance = -1;
		WaypointInfo result{};
		for (const auto& info : GetUnlockedWaypoints(targetSceneId)) {
			float distance = app::Vector3_Distance(nullptr, position, info.position, nullptr);
			if (minDistance < 0 || distance < minDistance)
			{
				minDistance = distance;
				result = info;
			}
		}
		return result;
	}

	uint32_t GetCurrentPlayerSceneID()
	{
		auto playerModule = GetSingleton(PlayerModule);
		if (playerModule == nullptr)
			return 0;

		return playerModule->fields.curSceneID;
	}

	uint32_t GetCurrentMapSceneID()
	{
		auto mapManager = GetSingleton(MapManager);
		if (mapManager == nullptr)
			return 0;

		return mapManager->fields.mapSceneID;
	}
}
