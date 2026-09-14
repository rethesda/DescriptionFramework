#include "../config/Configuration.h"
#include "../config/Settings.h"
#include "ItemCardFixer.h"
#include "ItemCardHook.h"

namespace
{
	std::string debugDescription = "<font color='#AAFF33'> My Custom Green Text <font color=\"#FFFFFF\">(This text is white)</font>  Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam risus massa, tincidunt nec pulvinar non, porttitor quis augue. Sed lacinia risus justo, eu pulvinar nulla dignissim nec.";

	std::string getDescription(RE::TESForm* a_item)
	{
		logger::debug("Looking up {:x}", a_item ? a_item->formID : 0);
		if (Settings::IsDebug()) {
			logger::debug("Debug mode, using debug description");
			return debugDescription;
		}
		const auto settings = Settings::GetSingleton();
		auto& database = ConfigurationDatabase::GetSingleton();
		const auto desc = database.GetDescriptionForObject(a_item);
		if (desc.empty()) {
			return "";
		}

		const auto prefix = !settings->tweaks.prefix.empty() ? settings->tweaks.prefix + " " : "";
		const auto suffix = !settings->tweaks.suffix.empty() ? " " + settings->tweaks.suffix : "";
		const auto font = settings->GetFontString() + prefix + desc + suffix;

		logger::debug("Using description {}", font.c_str());
		return font;
	}

	// Embed description into the itemcard, our item card fixer will pull this out and apply it to items as needed
	void addDescription(RE::ItemCard* itemCard, const std::string& description)
	{
		const auto descriptionValue = RE::GFxValue(description);
		itemCard->obj.SetMember(ItemCardFixer::descriptionVar, descriptionValue);

		logger::debug("Adding description to itemCard with movie view {:p}", (void*) itemCard->view.get());
	}

	// Embed description tag into the itemcard, to ensure accurate itemcard locating
	void addTag(RE::ItemCard* itemCard)
	{
		itemCard->obj.SetMember(ItemCardFixer::descriptionTag, true);

		logger::debug("Adding description tag to itemCard with movie view {:p}", (void*) itemCard->view.get());
	}
}


namespace hooks::ItemCardHooks {
	static void embedItemCard(RE::ItemCard* itemCard, RE::TESBoundObject** item)
	{
		if (!itemCard || !item || !*item) {
			return;
		}

		if (const auto desc = getDescription(*item); !desc.empty()) {
			addDescription(itemCard, desc);
			addTag(itemCard);
		}
	}

	// Let the item card populate, then embed our description afterwards
	void ItemCardPopulateHook::thunk(RE::ItemCard* itemCard, RE::TESBoundObject** a_item, char a3)
	{
		func(itemCard, a_item, a3);
		embedItemCard(itemCard, a_item);
	}

	void ItemCardPopulateHook2::thunk(RE::ItemCard* itemCard, RE::TESForm* a_item)
	{
		func(itemCard, a_item);
		if (!itemCard || !a_item) {
			return;
		}

		if (const auto desc = getDescription(a_item); !desc.empty()) {
			addDescription(itemCard, desc);
			addTag(itemCard);
		}
	}
}
