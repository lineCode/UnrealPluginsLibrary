// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "XmlSerializer.generated.h"

/**
 * xml序列化器
 */
UCLASS(Blueprintable)
class PUGIXML_API UXmlSerializer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	TMap<FString, FString> GetSerializeNames();

	template<typename T> T SerailizeTo(class UXmlDocument* Document) {
		T Value = T();
		SerailizeStruct(T::StaticStruct(), &Value, Document);
		return Value;
	}

	/**
	* 序列化结构体、UObject对象、数组、Map
	* @param SerializeClass 序列化器类型
	* @param Document xml文件对象
	* @param Value 需要赋值的对象
	*/
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "SerailizeTo", CustomStructureParam = "Value"), Category = "Serialize")
	static void BlueprintSerailizeTo(TSubclassOf<UXmlSerializer> SerializeClass, class UXmlDocument* Document, int32 Value);

private:
#if WITH_EDITOR
	void SerailizeStruct(UStruct* Struct, void* Address, class UXmlDocument* Document);
	void SerailizeArray(FArrayProperty* ArrayProperty, void* Address, class UXmlDocument* Document);
	void SerailizeMap(FMapProperty* MapProperty, void* Address, class UXmlDocument* Document);
#endif
private:
	DECLARE_FUNCTION(execBlueprintSerailizeTo);
};
