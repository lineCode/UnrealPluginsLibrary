// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PugiXml_Implementation.hpp"
#include "XmlTypes.h"
#include "Widgets/Layout/Anchors.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(PugiXml, Log, All);


class FPugiXml : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
};


namespace pugi
{
	PUGIXML_API xml_document LoadAndCombine(const FString& Folder);

	PUGIXML_API xml_document Load(const FString& Path, bool bInclude = true, bool PrintLog = false);

	PUGIXML_API FString Source(const xml_node& InNode);

	PUGIXML_API FString Source(const xml_document& Document);
};