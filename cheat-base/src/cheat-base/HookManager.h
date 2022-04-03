#pragma once

#include <map>
#include <detours.h>

#define callOrigin(function, ...) \
	HookManager::getOrigin(function)(__VA_ARGS__, __func__)

class HookManager
{
public:
	template <typename Fn>
	static void install(Fn func, Fn handler) 
	{
		enable(func, handler);
		holderMap[reinterpret_cast<void*>(handler)] = reinterpret_cast<void*>(func);
	}

	template <typename Fn>
	[[nodiscard]] static Fn getOrigin(Fn handler, const char* callerName = nullptr) noexcept
	{
		if (holderMap.count(reinterpret_cast<void*>(handler)) == 0) {
			LOG_CRIT("Origin not found for handler: %s", callerName == nullptr ? "<Unknown>" : callerName);
			system("pause");
			exit(1);
		}
		return reinterpret_cast<Fn>(holderMap[reinterpret_cast<void*>(handler)]);
	}

	template <typename Fn>
	[[nodiscard]] static void detach(Fn handler) noexcept 
	{
		disable(handler);
		holderMap.erase(reinterpret_cast<void*>(handler));
	}

	static void detachAll() noexcept
	{
		for (const auto &[key, value] : holderMap) 
		{
			disable(key);
		}
		holderMap.clear();
	}

private:
	inline static std::map<void*, void*> holderMap{};

	template <typename Fn>
	static void disable(Fn handler)
	{
		Fn origin = getOrigin(handler);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)origin, handler);
		DetourTransactionCommit();
	}

	template <typename Fn>
	static void enable(Fn& func, Fn handler) 
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)func, handler);
		DetourTransactionCommit();
	}
};


