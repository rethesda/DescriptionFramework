#include "MenuHook.h"
#include "../config/Configuration.h"
#include "../config/Settings.h"
#include "ItemCardFixer.h"

namespace
{
	// Some UIs differ in the exact item card name (eg Vanilla and SkyUI), so
	// we must track and dynamically find itemcards to accomodate all UIs as best
	// we can

	inline std::vector<std::string> knownItemcardNames = {
		"_root.Menu_mc.itemCard", // SkyUI
		"_root.Menu_mc.ItemCard_mc", // Vanilla
		"_root.Menu.ItemInfo" // SkyUI, CraftingMenu
	};

	// Dynamically parses and discovers itemCard locations, using the embedded `DF_tag` from the item card hooks
	class DiscoverItemCard : public RE::GFxValue::ObjectVisitor
	{
	public:
		// Recursive lookup of the ItemCard, using our embedded tag
		void Visit(const char* a_name, const RE::GFxValue& a_val) override
		{
			if (foundItemCard) {
				return;
			}
			std::string tab_string(depth, '\t');

			if (a_val.IsObject()) {
				if (depth != maxDepth) {
					depth++;
					paths.emplace_back(a_name);
					a_val.VisitMembers(this);
					paths.pop_back();
					depth--;
				}
			}

			if (std::strcmp(a_name, ItemCardFixer::descriptionTag) == 0 && a_val.IsBool() && a_val.GetBool() == true) {
				foundItemCard = true;
				std::string fullPath = rootPath;
				for (const auto& path : paths) {
					if (std::strcmp(path.c_str(), "LastUpdateObj") == 0) {
						// Hardcoded object which holds recent updates, like description tag. Skip as it's not the actual item card
						// Full tag path during dynamic discovery ends up like `_root.Menu_mc.$itemcardname$.LastUpdateObj.DF_tag
						continue;
					}
					fullPath += "." + path;
				}
				knownItemcardNames.push_back(fullPath);
				logger::debug("Found full itemcard path {}", fullPath);
			}
		}
	private:
		bool foundItemCard = false;
		std::uint32_t maxDepth = 5;
		std::uint32_t depth = 0;
		std::vector<std::string> paths;
		std::string rootPath = "_root";
	};

	std::optional<RE::GFxValue> locateItemCard(const RE::IMenu* a_menu)
	{
		RE::GFxValue a_itemCard {};

		// Fast path, check already known/found itemcard names
		for (const std::string& itemCard : knownItemcardNames) {
			if (a_menu->uiMovie->GetVariable(&a_itemCard, itemCard.c_str())) {
				return a_itemCard;
			}
		}

		// Slow path, dynamically parse for the itemcard, then recheck known names
		RE::GFxValue root;
		a_menu->uiMovie->GetVariable(&root, "_root");
		DiscoverItemCard findItemCardFunc;

		root.VisitMembers(&findItemCardFunc);

		for (const std::string& itemCard : knownItemcardNames) {
			if (a_menu->uiMovie->GetVariable(&a_itemCard, itemCard.c_str())) {
				return a_itemCard;
			}
		}

		logger::debug("Unable to find item card");
		return std::nullopt;
	}

	void applyDescriptionToMenu(const RE::IMenu* a_menu)
	{
		const auto itemCard = locateItemCard(a_menu);
		if (!itemCard.has_value()) {
			logger::debug("Movie has no itemcard, cannot apply descriptions!");
			return;
		}

		logger::debug("Applying item description");
		ItemCardFixer::applyDescription(*itemCard);
	}
}


namespace hooks::MenuHooks
{
	RE::UI_MESSAGE_RESULTS InventoryMenuHook::thunk(RE::IMenu* a_menu, RE::UIMessage& a_message)
	{
		const auto result = func(a_menu, a_message);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS InventoryMenuHookTwo::thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
	{
		const auto result = func(a_menu, a_interval, a_currentTime);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS BarterMenuHook::thunk(RE::IMenu* a_menu, RE::UIMessage& a_message)
	{
		const auto result = func(a_menu, a_message);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS BarterMenuHookTwo::thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
	{
		const auto result = func(a_menu, a_interval, a_currentTime);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS ContainerMenuHook::thunk(RE::IMenu* a_menu, RE::UIMessage& a_message)
	{
		const auto result = func(a_menu, a_message);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS ContainerMenuHookTwo::thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
	{
		const auto result = func(a_menu, a_interval, a_currentTime);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS GiftMenuHook::thunk(RE::IMenu* a_menu, RE::UIMessage& a_message)
	{
		const auto result = func(a_menu, a_message);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS GiftMenuHookTwo::thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
	{
		const auto result = func(a_menu, a_interval, a_currentTime);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS MagicMenuHook::thunk(RE::IMenu* a_menu, RE::UIMessage& a_message)
	{
		const auto result = func(a_menu, a_message);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS MagicMenuHookTwo::thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
	{
		const  auto result = func(a_menu, a_interval, a_currentTime);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS CraftingMenuHook::thunk(RE::IMenu* a_menu, RE::UIMessage& a_message)
	{
		const auto result = func(a_menu, a_message);
		applyDescriptionToMenu(a_menu);
		return result;
	}

	RE::UI_MESSAGE_RESULTS CraftingMenuHookTwo::thunk(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
	{
		const auto result = func(a_menu, a_interval, a_currentTime);
		applyDescriptionToMenu(a_menu);
		return result;
	}
}

