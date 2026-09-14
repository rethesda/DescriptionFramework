#pragma once
#include "../config/Settings.h"

namespace hooks::MenuHooks
{
	struct InventoryMenuHook
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, RE::UIMessage& a_message);

		static inline std::uint32_t idx = 0x4;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::InventoryMenu, InventoryMenuHook>();
			auto success = func.address() != 0;
			logger::info("InventoryMenuHook status: {}", success);
		}
	};
	struct InventoryMenuHookTwo
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime);

		static inline std::uint32_t idx = 0x5;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::InventoryMenu, InventoryMenuHookTwo>();
			auto success = func.address() != 0;
			logger::info("InventoryMenuHookTwo status: {}", success);
		}
	};
	struct BarterMenuHook
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, RE::UIMessage& a_message);

		static inline std::uint32_t idx = 0x4;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::BarterMenu, BarterMenuHook>();
			auto success = func.address() != 0;
			logger::info("BarterMenuHook status: {}", success);
		}
	};
	struct BarterMenuHookTwo
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime);

		static inline std::uint32_t idx = 0x5;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::BarterMenu, BarterMenuHookTwo>();
			auto success = func.address() != 0;
			logger::info("BarterMenuHookTwo status: {}", success);
		}
	};
	struct ContainerMenuHook
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, RE::UIMessage& a_message);

		static inline std::uint32_t idx = 0x4;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::ContainerMenu, ContainerMenuHook>();
			auto success = func.address() != 0;
			logger::info("ContainerMenuHook status: {}", success);
		}
	};
	struct ContainerMenuHookTwo
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime);

		static inline std::uint32_t idx = 0x5;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::ContainerMenu, ContainerMenuHookTwo>();
			auto success = func.address() != 0;
			logger::info("ContainerMenuHookTwo status: {}", success);
		}
	};
	struct GiftMenuHook
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, RE::UIMessage& a_message);

		static inline std::uint32_t idx = 0x4;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::GiftMenu, GiftMenuHook>();
			auto success = func.address() != 0;
			logger::info("GiftMenuHook status: {}", success);
		}
	};
	struct GiftMenuHookTwo
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime);

		static inline std::uint32_t idx = 0x5;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::GiftMenu, GiftMenuHookTwo>();
			auto success = func.address() != 0;
			logger::info("GiftMenuHookTwo status: {}", success);
		}
	};
	struct MagicMenuHook
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, RE::UIMessage& a_message);

		static inline std::uint32_t idx = 0x4;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::MagicMenu, MagicMenuHook>();
			auto success = func.address() != 0;
			logger::info("MagicMenuHook status: {}", success);
		}
	};
	struct MagicMenuHookTwo
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime);

		static inline std::uint32_t idx = 0x5;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::MagicMenu, MagicMenuHookTwo>();
			auto success = func.address() != 0;
			logger::info("MagicMenuHookTwo status: {}", success);
		}
	};
	struct CraftingMenuHook
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, RE::UIMessage& a_message);

		static inline std::uint32_t idx = 0x4;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::CraftingMenu, CraftingMenuHook>();
			auto success = func.address() != 0;
			logger::info("CraftingMenuHook status: {}", success);
		}
	};
	struct CraftingMenuHookTwo
	{
		static RE::UI_MESSAGE_RESULTS thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime);

		static inline std::uint32_t idx = 0x5;

		static inline REL::Relocation<decltype(thunk)> func;

		static void Install()
		{
			stl::write_vfunc<RE::CraftingMenu, CraftingMenuHookTwo>();
			auto success = func.address() != 0;
			logger::info("CraftingMenuHookTwo status: {}", success);
		}
	};

	static constexpr auto trampolineHookCount = 0;

	inline void Install() {
		InventoryMenuHook::Install();
		InventoryMenuHookTwo::Install();
		BarterMenuHook::Install();
		BarterMenuHookTwo::Install();
		ContainerMenuHookTwo::Install();
		ContainerMenuHook::Install();
		GiftMenuHook::Install();
		GiftMenuHookTwo::Install();
		MagicMenuHook::Install();
		MagicMenuHookTwo::Install();
		CraftingMenuHook::Install();
		CraftingMenuHookTwo::Install();
	}
}

