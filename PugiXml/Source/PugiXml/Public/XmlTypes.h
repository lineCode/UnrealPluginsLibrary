// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PugiXml.h"
#include "XmlTypes.generated.h"

USTRUCT(BlueprintType, meta=(HasNativeMake = ""))
struct PUGIXML_API FXmlNode
{
	GENERATED_BODY()

	pugi::xml_node Node;
};

UCLASS(BlueprintType)
class PUGIXML_API UXmlDocument : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY()
	uint8 bExternal : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bExternal", FilePathFilter = "Xml File (*.xml)|*.xml", RelativeToGameDir), Category = "Document")
	FFilePath SourceFile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true, EditCondition = "!bExternal"), Category = "Document")
	FString Content;

	/*
	 * 创建Xml文件
	 * @param XmlPath Xml文件的绝对路径
	*/
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", DisplayName = "Make Xml", AdvancedDisplay = "1"), Category = "Xml")
	static UXmlDocument* Make(UObject* WorldContextObject, FString XmlPath);

	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "Child"), Category = "Xml")
	FXmlNode Child(FString XPath);

	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "Children"), Category = "Xml")
	TArray<FXmlNode> Children(FString XPath);

	UFUNCTION(BlueprintCallable)
	void ReloadContent();

	UFUNCTION(BlueprintCallable)
	void ReloadDocument();

	FString FullFilePath();

	const pugi::xml_document& Get() const { return Document; }

	virtual void PostLoad() override;

#if WITH_EDITOR

	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	FSimpleMulticastDelegate OnFileChanged;
private:
	
	void RegistDirectoryChangedCallback(UXmlDocument* Dodument, bool bRegist = true);
	void Private_OnFileChanged(const TArray<struct FFileChangeData>&);
	FDelegateHandle OnFileChangedHanle;
	virtual void BeginDestroy() override;
#endif

private:
	pugi::xml_document Document;
};