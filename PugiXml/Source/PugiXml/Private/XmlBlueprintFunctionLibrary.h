// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XmlTypes.h"
#include "XmlBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UXmlBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "IsValid (XmlNode)", CompactNodeTitle = "isValid?", BlueprintThreadSafe), Category = "Xml")
	static bool IsValid_XmlNode(const FXmlNode& Node);

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "String", AdvancedDisplay = "2", AutoCreateRefTerm = "Default"), Category = "Xml")
	static FString Attribute_String(const FXmlNode& Node, FString Attribute, FString Default);

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "Name", AdvancedDisplay = "2", AutoCreateRefTerm = "Default"), Category = "Xml")
	static FName Attribute_Name(const FXmlNode& Node, FString Attribute, FName Default);

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "Boolean", AdvancedDisplay = "2"), Category = "Xml")
	static bool Attribute_Bool(const FXmlNode& Node, FString Attribute, bool Default = false);

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "Float", AdvancedDisplay = "2"), Category = "Xml")
	static float Attribute_Float(const FXmlNode& Node, FString Attribute, float Default = 0.0f);

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, CompactNodeTitle = "Int", AdvancedDisplay = "2"), Category = "Xml")
	static int32 Attribute_Int(const FXmlNode& Node, FString Attribute, int32 Default = 0);

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast), Category = "Xml")
	static bool HasAttribute(const FXmlNode& Node, FString Attribute);


	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, DisplayName="Children", CompactNodeTitle = "Children"), Category = "Xml")
	static TArray<FXmlNode> NodeChildren(const FXmlNode& Node, FString XPath);

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, DisplayName = "Child", CompactNodeTitle = "Child"), Category = "Xml")
	static FXmlNode NodeChild(const FXmlNode& Node, FString XPath);


	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, DisplayName = "Text", CompactNodeTitle = "Text"), Category = "Xml")
	static FString NodeText(const FXmlNode& Node);


	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, DisplayName = "Vector", CompactNodeTitle = "Vector"), Category = "Xml")
	static FVector ReadVector(const FXmlNode& Node, FString Attribute = "loc");

	UFUNCTION(BlueprintPure, meta = (BlueprintAutocast, DisplayName = "Rotator", CompactNodeTitle = "Rotator"), Category = "Xml")
	static FRotator ReadRotator(const FXmlNode& Node, FString Attribute = "rot");
};
