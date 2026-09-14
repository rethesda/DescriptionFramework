#include "Configuration.h"
#include "../MergeMapperPluginAPI.h"
#include "../Utils.h"

static std::map<RE::FormID, std::pair<std::string, int>> tempMap;  // Temporary map while populating configs

static std::string sanitizeLine(std::string line) {
	// Strip comments
	auto trimmedLine = std::string(line); // trim the whitespace and check if the first character is # or ;
	std::erase(trimmedLine, ' ');
	if (trimmedLine[0] == '#' || trimmedLine[0] == ';') {
		return "";
	}

	return line;
}

template <class T>
T* GetFormFromString(std::string line)
{
	auto form = RE::TESForm::LookupByEditorID(line);
	if (form && form->As<T>()) {
		return form->As<T>();
	}

	if (line.find('~') == std::string::npos) {
		logger::error("		missing plugin: {}", line);
		return nullptr;
	}

	auto plugin = line.substr(line.find('~') + 1);
	RE::FormID formID = std::stoul(line.substr(0, line.find('~')), nullptr, 16);
	if (g_mergeMapperInterface) {
		auto mergeForm = g_mergeMapperInterface->GetNewFormID(plugin.c_str(), formID);
		plugin = mergeForm.first;
		formID = mergeForm.second;
	}

	form = RE::TESDataHandler::GetSingleton()->LookupForm(formID, plugin);
	if (form == nullptr) {
		logger::error("		invalid form ID: {}", line);
		return nullptr;
	}

	if (form->As<T>()) {
		return form->As<T>();
	}
	return nullptr;
}

void ConfigurationDatabase::parseLine(std::string line)
{
	line = sanitizeLine(line);
	if (line.empty()) {
		return;
	}
	logger::debug("	Parsing line {}", line);
	auto tokens = utils::split_string(line, '|');
	if (tokens.size() < 2) {
		logger::error("		Invalid line setup, not enough info: {}", line);
		return;
	}
	auto object = GetFormFromString<RE::TESForm>(tokens[0]);

	auto& description = tokens[1];
	int priority = 0;
	if (tokens.size() >= 3) {
		priority = std::stoi(tokens[2]);
	}
	if (!object || description.empty()) {
		return;
	}

	if (tempMap.contains(object->formID) && tempMap[object->formID].second >= priority) {
		auto& desc = tempMap[object->formID];
		logger::debug("		Entry is overwritten by \'{}\' with priority {}", desc.first, desc.second);
		return;
	}

	if (tempMap.contains(object->formID) && tempMap[object->formID].second < priority) {
		auto& desc = tempMap[object->formID];
		logger::debug("		Overwriting previous entry \'{}\' with priority {}", desc.first, desc.second);
	}

	tempMap[object->formID] = { description, priority };
	logger::debug("		Entry with description \"{}\" inserted successfully!", description);
}

void ConfigurationDatabase::parseConfigs(const std::filesystem::path& configFile)
{
	logger::info("Parsing file {}", configFile.string().c_str());
	std::fstream file;
	file.open(configFile, std::ios::in);
	if (!file.is_open()) {
		logger::error("Couldn't open file {}", configFile.string().c_str());
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		try {	
			parseLine(line);
		} catch (...) {
			logger::warn("	Error parsing line {}", line);
		}
	}
}

static bool filesystempathcompare(const std::filesystem::path& a_first, const std::filesystem::path& a_second) {
	// case insensitive filesystem path comparing
	// if there's an easier way to do this and you're reading this, feel free to shoot a PR!
	auto a_firstString = a_first.string();
	auto a_secondString = a_second.string();
	std::ranges::transform(std::as_const(a_firstString), a_firstString.begin(), [](unsigned char c) { return (char) std::tolower(c); });
	std::ranges::transform(std::as_const(a_secondString), a_secondString.begin(), [](unsigned char c) { return (char) std::tolower(c); });
	return a_firstString.compare(a_secondString) < 0;
}

void ConfigurationDatabase::Initialize() {
	logger::info("Reading descriptions configs...");

	constexpr auto path = L"Data";

	std::vector<std::filesystem::path> filePaths;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		try {
			if (!entry.path().filename().string().ends_with("_DESC.ini")) {
				continue;
			}
			 
			filePaths.push_back(entry.path());
		} catch(...) {}
	}

	std::ranges::sort(filePaths, filesystempathcompare);

	for (const auto& entry : filePaths) {
		try {
			logger::debug("Parsing {}", entry.string());
			parseConfigs(entry);
		} catch (...) {
			logger::error("Error parsing {}", entry.string());
		}	
	}

	// Convert tempMap to the description map
	for (auto& [formID, config] : tempMap) {
		logger::debug("Emplacing {:x} with {}", formID, config.first);
		descriptionMap.emplace(formID, config.first);
	}

	logger::info("Config APIs fully parsed!");
}

std::string ConfigurationDatabase::GetDescriptionForObject(RE::TESForm* a_object)
{
	if (!a_object) {
		return "";
	}

	std::lock_guard mapLock(descriptionMutex);

	if (scriptDescriptionMap.contains(a_object->formID)) {
		return scriptDescriptionMap[a_object->formID]; 
	}

	if (descriptionMap.contains(a_object->formID)) {
		logger::debug("Using entry {} for {:x}", descriptionMap[a_object->formID], a_object->formID);
		return descriptionMap[a_object->formID];
	}

	if (a_object->As<RE::BGSKeywordForm>()) {
		auto keywordList = a_object->As<RE::BGSKeywordForm>();
		for (auto keyword : keywordList->GetKeywords()) {
			if (auto keywordDESC = GetDescriptionForObject(keyword); !keywordDESC.empty()) {
				return keywordDESC;
			}
		}
	}

	return "";
}

std::string ConfigurationDatabase::GetScriptDescriptionForObject(const RE::TESForm* a_object)
{
	if (!a_object) {
		return "";
	}

	std::lock_guard mapLock(descriptionMutex);

	if (scriptDescriptionMap.contains(a_object->formID)) {
		return scriptDescriptionMap[a_object->formID];
	}

	return "";
}

// Set description from papyrus
void ConfigurationDatabase::SetScriptDescriptionForObject(const RE::TESForm* a_object, std::string a_description)
{
	if (!a_object) {
		return;
	}

	std::lock_guard mapLock(descriptionMutex);

	if (scriptDescriptionMap.contains(a_object->formID)) {
		scriptDescriptionMap.erase(a_object->formID);
	}

	scriptDescriptionMap[a_object->formID] = a_description;
}

// Reset description from papyrus
void ConfigurationDatabase::ResetScriptDescriptionForObject(RE::TESForm* a_object) {
	if (!a_object) {
		return;
	}

	std::lock_guard mapLock(descriptionMutex);

	if (scriptDescriptionMap.contains(a_object->formID)) {
		scriptDescriptionMap.erase(a_object->formID);
	}
}
