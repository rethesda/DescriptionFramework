#pragma once

class ConfigurationDatabase
{
public:
	ConfigurationDatabase(const ConfigurationDatabase&) = delete;
	void operator=(const ConfigurationDatabase&) = delete;

	static ConfigurationDatabase& GetSingleton()
	{
		static ConfigurationDatabase instance;
		return instance;
	}

	// Load entries from the various entry files
	void Initialize();

	// Get object's description. Script descriptions take priority over descriptions from configuration files
	std::string GetDescriptionForObject(RE::TESForm* a_object);

	// Get object's description if it was set by a script
	std::string GetScriptDescriptionForObject(const RE::TESForm* a_object);

	// Set description from papyrus
	void SetScriptDescriptionForObject(const RE::TESForm* a_object, std::string a_description);

	// Set from papyrus by a mod
	void ResetScriptDescriptionForObject(RE::TESForm* a_object);

private:
	ConfigurationDatabase()
	{
		Initialize();
	}
	void parseConfigs(const std::filesystem::path& configFile);
	void parseLine(std::string line);
	std::recursive_mutex descriptionMutex;
	std::map<RE::FormID, std::string> descriptionMap;                      // Map of descriptions as determined by initial configuration files
	std::map<RE::FormID, std::string> scriptDescriptionMap;                // Map of descriptions as set from Papyrus at runtime, these always take priority
};
