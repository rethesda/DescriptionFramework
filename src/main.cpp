#include "MergeMapperPluginAPI.h"
#include "Version.h"
#include "api/DescriptionFrameworkAPI.h"
#include "api/Papyrus.h"
#include "config/Configuration.h"
#include "config/Settings.h"
#include "hook/ItemCardHook.h"
#include "hook/MenuHook.h"

namespace DescriptionFrameworkAPI
{
	// Handles skse mod messages requesting to fetch API functions from DescriptionFramework
	void ModMessageHandler(SKSE::MessagingInterface::Message* message);

	// This object provides access to DescriptionFramework's mod support API version 1
	struct DescriptionFrameworkInterface001 : IDescriptionFrameworkInterface001
	{
		unsigned int GetBuildNumber() override {
			return (Project::Version::MAJOR >> 8) + (Project::Version::MINOR >> 4) + Project::Version::PATCH;
		}

		const char* GetDescription(RE::TESForm* a_form) override
		{
			// Recycle result for each thread call
			thread_local std::string* result = nullptr;
			delete result;

			result = new std::string(ConfigurationDatabase::GetSingleton().GetDescriptionForObject(a_form));
			return result->c_str();
		}
	};

}  // namespace SkyrimVRESLPluginAPI
extern DescriptionFrameworkAPI::DescriptionFrameworkInterface001 g_interface001;

DescriptionFrameworkAPI::DescriptionFrameworkInterface001 g_interface001;

// Constructs and returns an API of the revision number requested
void* GetApi(unsigned int revisionNumber)
{
	switch (revisionNumber) {
	case 1:
		logger::info("Interface revision 1 requested");
		return &g_interface001;
	default:;
	}
	return nullptr;
}

// Handles skse mod messages requesting to fetch API functions
void ModMessageHandler(SKSE::MessagingInterface::Message* message)
{
	using namespace DescriptionFrameworkAPI;
	if (message->type == DescriptionFrameworkMessage::kMessage_GetInterface) {
		DescriptionFrameworkMessage* modmappermessage = (DescriptionFrameworkMessage*)message->data;
		modmappermessage->GetApiFunction = GetApi;
		logger::info("Provided DescriptionFramework plugin interface to {}", message->sender);
	}
}

void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
	switch (a_message->type) {
	case SKSE::MessagingInterface::kPostLoad: {
			if (SKSE::GetMessagingInterface()->RegisterListener(nullptr, ModMessageHandler))
				logger::info("Successfully registered SKSE listener {} with buildnumber {}",
					DescriptionFrameworkAPI::DescriptionFrameworkPluginName, g_interface001.GetBuildNumber());
			else
				logger::info("Unable to register SKSE listener");
			break;
	}
	case SKSE::MessagingInterface::kPostPostLoad:
		{
			logger::info("Dependencies check...");
			if (!GetModuleHandle(L"po3_Tweaks")) {
				logger::critical("po3_Tweaks not detected, mod will not function right!");
			}
			MergeMapperPluginAPI::GetMergeMapperInterface001();  // Request interface
			if (g_mergeMapperInterface) {                        // Use Interface
				const auto version = g_mergeMapperInterface->GetBuildNumber();
				logger::info("Got MergeMapper interface buildnumber {}", version);
			} else
				logger::info("MergeMapper not detected");
			logger::info("Dependencies check complete!");
			break;
		}
	case SKSE::MessagingInterface::kDataLoaded:
		{
			ConfigurationDatabase::GetSingleton();
		}
	default:
		break;
	}
}

SKSEPluginInfo(
	.Version = {Project::Version::MAJOR, Project::Version::MINOR, Project::Version::PATCH},
	.Name = Project::NAME,
	.Author = Project::AUTHOR,
	.SupportEmail = "N/A",
	.StructCompatibility = SKSE::StructCompatibility::Independent,
)

extern "C" DLLEXPORT const char* APIENTRY GetPluginVersion()
{
	return Project::Version::NAME.data();
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	auto logLevel = spdlog::level::info;
	if (Settings::IsDebug()) {
		logLevel = spdlog::level::debug;
	}
	const auto initInfo = SKSE::InitInfo {
		.log = true,
		.logLevel = logLevel,
		.trampoline = true,
		.trampolineSize = (hooks::ItemCardHooks::trampolineHookCount + hooks::MenuHooks::trampolineHookCount) * 0x14
	};
	SKSE::Init(a_skse, initInfo);

	Settings::GetSingleton()->Load();

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);

	auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(Papyrus::Bind);

	hooks::ItemCardHooks::Install();
	hooks::MenuHooks::Install();

	return true;
}
