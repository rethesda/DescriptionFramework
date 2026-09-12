#include "hooks.h"
#include "Configuration.h"
#include "MergeMapperPluginAPI.h"
#include "Settings.h"
#include "Papyrus.h"
#include "DescriptionFrameworkAPI.h"
#include "Version.h"

namespace DescriptionFrameworkAPI
{
	// Handles skse mod messages requesting to fetch API functions from DescriptionFramework
	void ModMessageHandler(SKSE::MessagingInterface::Message* message);

	// This object provides access to DescriptionFramework's mod support API version 1
	struct DescriptionFrameworkInterface001 : IDescriptionFrameworkInterface001
	{
		virtual unsigned int GetBuildNumber() {
			return (Project::Version::MAJOR >> 8) + (Project::Version::MINOR >> 4) + Project::Version::PATCH;
		};

		virtual const char* GetDescription(RE::TESForm* a_form)
		{
			const auto result = new std::string(ConfigurationDatabase::GetSingleton()->GetDescriptionForObject(a_form));
			return result->c_str();
		};
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
			ConfigurationDatabase::GetSingleton()->Initialize();
		}
	default:
		break;
	}
}

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= Project::NAME;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	if (Settings::IsDebug()) {
		log->set_level(spdlog::level::debug);
		log->flush_on(spdlog::level::debug);
	}
	else {
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
	}
	
	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Project::NAME, Project::Version::NAME);
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
	Settings::GetSingleton()->Load();
	InitializeLog();
	logger::info("loaded plugin");

	constexpr auto initInfo = SKSE::InitInfo {.trampoline = true, .trampolineSize = hooks::trampolineHookCount * 0x14 };
	SKSE::Init(a_skse, initInfo);

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);

	auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(Papyrus::Bind);

	hooks::InstallHooks();

	return true;
}
