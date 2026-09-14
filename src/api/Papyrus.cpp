#include "Papyrus.h"
#include "../config/Configuration.h"
#include "Version.h"

namespace
{
	std::optional<RE::TESBoundObject*> GetBaseObjectSafe(RE::TESForm* a_object)
	{
		if (!a_object) {
			return std::nullopt;
		}

		if (const auto referenceObject = a_object->As<RE::TESObjectREFR>(); referenceObject != nullptr) {
			if (referenceObject->GetBaseObject()) {
				return referenceObject->GetBaseObject();
			}
			return std::nullopt;
		}

		if (const auto asBoundObject = a_object->As<RE::TESBoundObject>(); asBoundObject != nullptr) {
			return asBoundObject;
		}

		return std::nullopt;
	}
}

namespace Papyrus
{
	static std::vector<std::int32_t> GetVersion(RE::StaticFunctionTag*)
	{
		return { Project::Version::MAJOR, Project::Version::MINOR, Project::Version::PATCH };
	}

	static RE::BSFixedString GetDescription(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_object) {
		const auto a_base = GetBaseObjectSafe(a_object);
		if (!a_base.has_value()) {
			a_vm->TraceStack("Base object is empty", a_stackID);
			return "";
		}
		return ConfigurationDatabase::GetSingleton().GetDescriptionForObject(*a_base);
	}

	static RE::BSFixedString GetScriptedDescription(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_object)
	{
		const auto a_base = GetBaseObjectSafe(a_object);
		if (!a_base.has_value()) {
			a_vm->TraceStack("Base object is empty", a_stackID);
			return "";
		}
		return ConfigurationDatabase::GetSingleton().GetScriptDescriptionForObject(*a_base);
	}

	static void SetDescription(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_object, RE::BSFixedString a_desc)
	{
		auto a_base = GetBaseObjectSafe(a_object);
		if (!a_base.has_value()) {
			a_vm->TraceStack("Base object is empty", a_stackID);
			return;
		}

		ConfigurationDatabase::GetSingleton().SetScriptDescriptionForObject(*a_base, std::string(a_desc));
	}

	void ResetDescription(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESForm* a_object)
	{
		auto a_base = GetBaseObjectSafe(a_object);
		if (!a_base.has_value()) {
			a_vm->TraceStack("Base object is empty", a_stackID);
			return;
		}

		ConfigurationDatabase::GetSingleton().ResetScriptDescriptionForObject(*a_base);
	}

	bool Bind(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("couldn't get VM State"sv);
			return false;
		}

		logger::info("Binding functions..."sv);

		
		BIND(GetVersion, true); 

		logger::info("Registered GetVersion"sv);

		BIND(GetDescription);

		logger::info("Registed GetDescription"sv);

		BIND(GetScriptedDescription);

		logger::info("Registed GetScriptedDescription"sv);

		BIND(SetDescription);

		logger::info("Registed SetDescription"sv);

		BIND(ResetDescription);

		logger::info("Registed ResetDescription"sv);

		return true;
	}
}
