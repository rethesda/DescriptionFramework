#pragma once
#include "Configuration.h"

namespace hooks
{
	struct ItemCardPopulateHook
	{
		static inline std::uint32_t ICT_ARMOR = 1;
		static inline std::uint32_t ICT_BOOK = 4;
		static inline std::uint32_t ICT_POTION = 6;
		

		static void thunk(RE::ItemCard* itemCard, RE::TESBoundObject** a_item, char a3)
		{
			if (!itemCard || !a_item || !*a_item || getDescription(*a_item) == "") {
				return func(itemCard, a_item, a3);
			}

			func(itemCard, a_item, a3);
			handleMiscItems(itemCard, *a_item);
			handleSoulGems(itemCard, *a_item);
			handleIngredients(itemCard, *a_item);
			handleEffectsItems(itemCard, *a_item);
			handleDescriptionItems(itemCard, *a_item);
		};

		static const char* getDescription(RE::TESBoundObject* a_item)
		{
			auto database = ConfigurationDatabase::GetSingleton();
			if (!database->GetConfigurationForObject(a_item)) {
				return "";
			}
			return database->GetConfigurationForObject(a_item)->description.c_str();
		}

		static void handleMiscItems(RE::ItemCard* itemCard, RE::TESBoundObject* a_item)
		{
			if (a_item->Is(RE::FormType::Misc)) {
				// Fake the MISC object as a BOOK so the UI will load the description label
				auto typeValue = RE::GFxValue(ICT_BOOK);
				itemCard->obj.SetMember("type", typeValue);

				auto emptyValue = RE::GFxValue("");
				itemCard->obj.SetMember("skillText", emptyValue);
			}
		}

		static void handleEffectsItems(RE::ItemCard* itemCard, RE::TESBoundObject* a_item)
		{
			// TODO: Armor and Weapons show as enchanted if given an effect, excluding for now until we fix
			if (a_item->Is(RE::FormType::Weapon) || a_item->Is(RE::FormType::Armor)) {
				return;
			}

			// Potions, Food (Plus any items faked as a potion/food)
			RE::GFxValue origEffect = RE::GFxValue("");
			std::string origEffectDesc = "";
			if (itemCard->obj.HasMember("effects") && itemCard->obj.GetMember("effects", &origEffect)) {
				std::string origString = origEffect.GetString();
				origString.erase(remove(origString.begin(), origString.end(), ' '), origString.end());
				if (!origString.empty()) {
					origEffectDesc = std::string(origEffect.GetString()) + "\n";
				}
			}
			auto effectsDesc = origEffectDesc + getDescription(a_item);
			auto effectsValue = RE::GFxValue(effectsDesc);
			itemCard->obj.SetMember("effects", effectsValue);
		}

		static void handleDescriptionItems(RE::ItemCard* itemCard, RE::TESBoundObject* a_item)
		{
			// Books (Plus any items faked as a book)
			auto descriptionValue = RE::GFxValue(getDescription(a_item));
			itemCard->obj.SetMember("description", descriptionValue);
		}

		static void handleIngredients(RE::ItemCard* itemCard, RE::TESBoundObject* a_item)
		{
			// TODO: Trick ingredients as a potion, and mimic the same 4 effect labels with a description?
			if (a_item->Is(RE::FormType::Ingredient)) {
			}
		}

		static void handleSoulGems(RE::ItemCard* itemCard, RE::TESBoundObject* a_item)
		{
			// TODO: Trick soul gems as a potion to get HTML text benefit?
			if (a_item->Is(RE::FormType::SoulGem)) {
			}
		}

		static inline REL::Relocation<decltype(thunk)> func;

		// Install our hook at the specified address
		static inline void Install()
		{
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(50005, 50949), REL::VariantOffset(0x80, 0x80, 0x80) };
			stl::write_thunk_call<ItemCardPopulateHook>(target.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target.offset());

			REL::Relocation<std::uintptr_t> target2{ RELOCATION_ID(50201, 51130), REL::VariantOffset(0xB2, 0xB2, 0xB2) };
			stl::write_thunk_call<ItemCardPopulateHook>(target2.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target2.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target2.offset());

			REL::Relocation<std::uintptr_t> target3{ RELOCATION_ID(50297, 51218), REL::VariantOffset(0x35, 0x35, 0x35) };
			stl::write_thunk_call<ItemCardPopulateHook>(target3.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target3.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target3.offset());

			REL::Relocation<std::uintptr_t> target4{ RELOCATION_ID(50674, 51569), REL::VariantOffset(0x80, 0x7A, 0x80) };
			stl::write_thunk_call<ItemCardPopulateHook>(target4.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target4.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target4.offset());

			REL::Relocation<std::uintptr_t> target5{ RELOCATION_ID(50973, 51852), REL::VariantOffset(0x80, 0x7A, 0x80) };
			stl::write_thunk_call<ItemCardPopulateHook>(target5.address());

			logger::info("ItemCardPopulateHook hooked at address {:x}", target5.address());
			logger::info("ItemCardPopulateHook hooked at offset {:x}", target5.offset());

			if (REL::Module::IsAE()) {
				REL::Relocation<std::uintptr_t> target6{ RELOCATION_ID(0, 51458), REL::VariantOffset(0x0, 0x87, 0x0) };
				stl::write_thunk_call<ItemCardPopulateHook>(target6.address());

				logger::info("ItemCardPopulateHook hooked at address {:x}", target6.address());
				logger::info("ItemCardPopulateHook hooked at offset {:x}", target6.offset());
			}
		}
	};

	static inline void InstallHooks()
	{
		ItemCardPopulateHook::Install();
	}
}
