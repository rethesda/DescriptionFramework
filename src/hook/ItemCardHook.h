#pragma once

namespace hooks::ItemCardHooks {
	struct ItemCardPopulateHook
	{
		static auto constexpr trampolineHookCount = 6;
		// Let the item card populate, then embed our description afterwards
		static void thunk(RE::ItemCard* itemCard, RE::TESBoundObject** a_item, char a3);

		static inline REL::Relocation<decltype(thunk)> func;

		// Install our hook at the specified address
		static void Install()
		{
			const REL::Relocation target{ RELOCATION_ID(50005, 50949), REL::VariantOffset(0x80, 0x80, 0x80) };
			stl::write_thunk_call<ItemCardPopulateHook>(target.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target.offset());

			const REL::Relocation target2{ RELOCATION_ID(50201, 51130), REL::VariantOffset(0xB2, 0xB2, 0xB2) };
			stl::write_thunk_call<ItemCardPopulateHook>(target2.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target2.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target2.offset());

			const REL::Relocation target3{ RELOCATION_ID(50297, 51218), REL::VariantOffset(0x35, 0x35, 0x35) };
			stl::write_thunk_call<ItemCardPopulateHook>(target3.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target3.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target3.offset());

			const REL::Relocation target4{ RELOCATION_ID(50674, 51569), REL::VariantOffset(0x80, 0x7A, 0x80) };
			stl::write_thunk_call<ItemCardPopulateHook>(target4.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target4.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target4.offset());

			const REL::Relocation target5{ RELOCATION_ID(50973, 51852), REL::VariantOffset(0x80, 0x7A, 0x80) };
			stl::write_thunk_call<ItemCardPopulateHook>(target5.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target5.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target5.offset());

			if (REL::Module::IsAE()) {
				const REL::Relocation target6{ RELOCATION_ID(0, 51458), REL::VariantOffset(0x0, 0x87, 0x0) };
				stl::write_thunk_call<ItemCardPopulateHook>(target6.address());

				logger::info("ItemCardPopulateHook hooked at address {:x}", target6.address());
				logger::info("ItemCardPopulateHook hooked at offset {:x}", target6.offset());
			}
		}
	};

	struct ItemCardPopulateHook2
	{
		static auto constexpr trampolineHookCount = 2;
		// Let the item card populate, then embed our description afterwards
		static void thunk(RE::ItemCard* itemCard, RE::TESForm* a_item);

		static inline REL::Relocation<decltype(thunk)> func;

		// Install our hook at the specified address
		static void Install()
		{
			const REL::Relocation target{ RELOCATION_ID(50298, 51219), REL::VariantOffset(0x32, 0x32, 0x32) };
			stl::write_thunk_call<ItemCardPopulateHook2>(target.address());

			logger::info("ItemCardPopulateHook2 hooked at address {:x}", target.address());
			logger::info("ItemCardPopulateHook2 hooked at offset {:x}", target.offset());

			const REL::Relocation target2{ RELOCATION_ID(51152, 52032), REL::VariantOffset(0x8F, 0x8F, 0x8B) };
			stl::write_thunk_call<ItemCardPopulateHook2>(target2.address());

			logger::info("ItemCardPopulateHook2 hooked at address {:x}", target2.address());
			logger::info("ItemCardPopulateHook2 hooked at offset {:x}", target2.offset());
		}
	};

	static auto constexpr trampolineHookCount = ItemCardPopulateHook::trampolineHookCount + ItemCardPopulateHook2::trampolineHookCount;
	inline void Install()
	{
		ItemCardPopulateHook::Install();
		ItemCardPopulateHook2::Install();
	}
}

