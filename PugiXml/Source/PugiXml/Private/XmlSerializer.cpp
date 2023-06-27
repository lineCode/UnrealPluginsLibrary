// Fill out your copyright notice in the Description page of Project Settings.


#include "XmlSerializer.h"
#include "XmlTypes.h"

enum class EInnerPropertyType : uint8
{
	Default = 0,
	Int,
	Float,
	Double,
	Bool,
	String,
	Text,
	Name
};

TMap<FString, FString> UXmlSerializer::GetSerializeNames_Implementation()
{
	TMap<FString, FString> Map;
	return MoveTemp(Map);
}

void UXmlSerializer::BlueprintSerailizeTo(TSubclassOf<UXmlSerializer> SerializeClass, UXmlDocument* Document, int32 Value)
{
	check(0);
}

static FString GetArrayName(const FString& InName)
{
	auto Ret = InName;
	if (Ret.EndsWith("s")) {
		Ret.RemoveFromEnd("s");
	}
	return MoveTemp(Ret);
}


static EInnerPropertyType PropertyType(FProperty* Property);

#if WITH_EDITOR
static void SerailizeNodeToStruct(FStructProperty* Property, void* Address, const pugi::xml_node& Node);
static void SerailizeNodeToArray(FArrayProperty* Property, void* Address, const pugi::xml_object_range<pugi::xml_named_node_iterator>& Children);
static void SerailizeNodeToMap(FMapProperty* Property, const FString& KeyPropertyName, void* Address, const pugi::xml_object_range<pugi::xml_named_node_iterator>& Children);
#endif
/**
* 根据Map属性获取在对应XML文件中查找数据时的Key Value信息
* @param Property Map属性
* @param KeyName 查找时Map的Key
* @param ValueName 查找时Map的Value
* @return 是否分割成功
*/
static bool SplitMapInfo(FMapProperty* Property, FString& KeyName, FString& ValueName);

/**
* 转化xml_object_range为TArray<node>类型
* @param Items xml_object_range
* @return 转化的Array
*/
static TArray<pugi::xml_node> ConvertXmlRangeToArray(const pugi::xml_object_range<pugi::xml_named_node_iterator>& Items);

/**
* 获取节点属性值,若无该属性则尝试获取子节点文本
*/
static const wchar_t* NodePropertyAsString(const pugi::xml_node& Node, const wchar_t* KeyName)
{
	const wchar_t* RetValue = Node.attribute(KeyName).as_string();
	if (FString(RetValue).IsEmpty())
	{
		if (Node.child(KeyName))
		{
			RetValue = Node.child(KeyName).text().as_string();
		}
	}
	return RetValue;
}

EInnerPropertyType PropertyType(FProperty* Property)
{
	if (Property->IsA<FIntProperty>())
	{
		return EInnerPropertyType::Int;
	}
	else if (Property->IsA<FFloatProperty>())
	{
		return EInnerPropertyType::Float;
	}
	else if (Property->IsA<FDoubleProperty>())
	{
		return EInnerPropertyType::Double;
	}
	else if (Property->IsA<FBoolProperty>())
	{
		return EInnerPropertyType::Bool;
	}
	else if (Property->IsA<FStrProperty>())
	{
		return EInnerPropertyType::String;
	}
	else if (Property->IsA<FTextProperty>())
	{
		return EInnerPropertyType::Text;
	}
	else if (Property->IsA<FNameProperty>())
	{
		return EInnerPropertyType::Name;
	}
	return EInnerPropertyType::Default;
}

#if WITH_EDITOR

static void SerailizeNodeToStruct(FStructProperty* Property, void* Address, const pugi::xml_node& Node)
{
	for (auto It = TFieldIterator<FProperty>(Property->Struct); It; ++It)
	{
		auto Name = It->GetDisplayNameText().ToString().ToLower();

		uint8* NewAddress = (uint8*)Address + It->GetOffset_ForInternal();

		if (It->IsA<FIntProperty>()) {
			auto Value = Node.attribute(*Name).as_int();
			CastField<FIntProperty>(*It)->SetPropertyValue(NewAddress, Value);
		}
		else if (It->IsA<FBoolProperty>()) {
			auto Value = Node.attribute(*Name).as_bool();
			CastField<FBoolProperty>(*It)->SetPropertyValue(NewAddress, Value);
		}
		else if (It->IsA<FFloatProperty>()) {
			auto Value = Node.attribute(*Name).as_float();
			CastField<FFloatProperty>(*It)->SetPropertyValue(NewAddress, Value);
		}
		else if (It->IsA<FDoubleProperty>()) {
			auto Value = Node.attribute(*Name).as_bool();
			CastField<FDoubleProperty>(*It)->SetPropertyValue(NewAddress, Value);
		}
		else if (It->IsA<FStrProperty>()) {
			FString Value = NodePropertyAsString(Node, *Name);
			CastField<FStrProperty>(*It)->SetPropertyValue(NewAddress, Value);
		}
		else if (It->IsA<FNameProperty>()) {
			FName Value = NodePropertyAsString(Node, *Name);
			CastField<FNameProperty>(*It)->SetPropertyValue(NewAddress, Value);
		}
		else if (It->IsA<FTextProperty>()) {
			FText Value = FText::FromString(NodePropertyAsString(Node, *Name));
			CastField<FTextProperty>(*It)->SetPropertyValue(NewAddress, Value);
		}
		else if (It->IsA<FStructProperty>()) {
			auto Child = Node.child(*Name);
			if (Child) {
				SerailizeNodeToStruct(CastField<FStructProperty>(*It), NewAddress, Child);
			}
		}
		else if (It->IsA<FArrayProperty>()) {
			Name = GetArrayName(Name);
			auto Children = Node.children(*Name);
			SerailizeNodeToArray(CastField<FArrayProperty>(*It), NewAddress, Children);
		}
		else if (It->IsA<FMapProperty>()) {
			auto MapProperty = CastField<FMapProperty>(*It);
			FString KeyName;
			FString ValueName;
			if (SplitMapInfo(MapProperty, KeyName, ValueName))
			{
				auto Children = Node.children(*Name);
				SerailizeNodeToMap(MapProperty, "", Address, Children);
			}
		}
	}
}

static void SerailizeNodeToArray(FArrayProperty* Property, void* Address, const pugi::xml_object_range<pugi::xml_named_node_iterator>& Children)
{
	FScriptArrayHelper Helper(Property, Address);

	FProperty* InnerProperty = Property->Inner;
	if (!InnerProperty->IsA<FStructProperty>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Array: Value Must Be A UStruct Type"));
		return;
	}
	auto StructProperty = CastField<FStructProperty>(Property->Inner);

	for (auto& Child : Children)
	{
		auto Ptr = Helper.GetRawPtr(Helper.AddValue());
		SerailizeNodeToStruct(StructProperty, Ptr, Child);
	}
}

#endif
void SerailizeNodeToMap(FMapProperty* Property, const FString& KeyPropertyName, void* Address, const pugi::xml_object_range<pugi::xml_named_node_iterator>& Children)
{
	FProperty* KeyProperty = Property->KeyProp;
	FProperty* ValueProperty = Property->ValueProp;

	EInnerPropertyType InnerPropertyType = PropertyType(KeyProperty);
	if (InnerPropertyType == EInnerPropertyType::Default)
	{
		return;
	}

	FScriptMapHelper Helper(Property, Address);

	TArray<pugi::xml_node> ChildItems = ConvertXmlRangeToArray(Children);
	Helper.EmptyValues(ChildItems.Num());

	// temporary buffers to read elements into
	TArray<uint8> TempKey;
	TempKey.AddZeroed(KeyProperty->ElementSize);
	TArray<uint8> TempValue;
	TempValue.AddZeroed(ValueProperty->ElementSize);

	for (int Index = 0; Index < ChildItems.Num(); Index++)
	{
		auto ChildNode = ChildItems[Index];
		KeyProperty->InitializeValue(TempKey.GetData());
		switch (InnerPropertyType)
		{
		case EInnerPropertyType::Int:
			CastField<FIntProperty>(KeyProperty)->SetPropertyValue(TempKey.GetData(), ChildNode.attribute(*KeyPropertyName).as_int());
			break;
		case EInnerPropertyType::String:
			CastField<FStrProperty>(KeyProperty)->SetPropertyValue(TempKey.GetData(), NodePropertyAsString(ChildNode, *KeyPropertyName));
			break;
		case EInnerPropertyType::Text:
			CastField<FTextProperty>(KeyProperty)->SetPropertyValue(TempKey.GetData(),FText::FromString(NodePropertyAsString(ChildNode, *KeyPropertyName)));
			break;
		case EInnerPropertyType::Name:
			CastField<FNameProperty>(KeyProperty)->SetPropertyValue(TempKey.GetData(), NodePropertyAsString(ChildNode, *KeyPropertyName));
			break;
		default:
			break;
		}

		ValueProperty->InitializeValue(TempValue.GetData());
#if WITH_EDITOR
		SerailizeNodeToStruct(CastField<FStructProperty>(ValueProperty), TempValue.GetData(), ChildNode);
#endif
		Helper.AddPair(TempKey.GetData(), TempValue.GetData());

		KeyProperty->DestroyValue(TempKey.GetData());
		ValueProperty->DestroyValue(TempValue.GetData());
	}
}

bool SplitMapInfo(FMapProperty* MapProperty, FString& KeyName, FString& ValueName)
{
	if (!MapProperty->ValueProp->IsA<FStructProperty>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Map: Value Must Be A UStruct Type"));
		return false;
	}
#if WITH_EDITOR
	FString PropertyName = MapProperty->GetDisplayNameText().ToString().ToLower();
	PropertyName.RemoveSpacesInline();
	if (!PropertyName.Split("to", &KeyName, &ValueName, ESearchCase::Type::IgnoreCase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Map: Name Must Be XXXToXXX"));
		return false;
	}
#endif
	return true;
}

TArray<pugi::xml_node> ConvertXmlRangeToArray(const pugi::xml_object_range<pugi::xml_named_node_iterator>& Items)
{
	TArray<pugi::xml_node> NodeItems;
	for (auto& Item : Items)
	{
		NodeItems.Add(Item);
	}
	return MoveTemp(NodeItems);
}

#if WITH_EDITOR

void UXmlSerializer::SerailizeStruct(UStruct* Struct, void* Address, UXmlDocument* Document)
{
	auto Names = GetSerializeNames();
	auto Root = Document->Get().first_child();
	for (auto It = TFieldIterator<FProperty>(Struct); It; ++It)
	{
		auto PropertyName = It->GetDisplayNameText().ToString();
		PropertyName = PropertyName.ToLower();

		if (It->IsA<FArrayProperty>()) 
		{
			PropertyName = GetArrayName(PropertyName);
			auto Children = Root.children(*PropertyName);

			auto ArrayProperty = CastField<FArrayProperty>(*It);
			SerailizeNodeToArray(ArrayProperty, Address, Children);
		}
		else if (It->IsA<FStructProperty>()) 
		{
			auto Children = Root.child(*PropertyName);

			auto StructProperty = CastField<FStructProperty>(*It);
			SerailizeNodeToStruct(StructProperty, Address, *Children.begin());
		}
		else if (It->IsA<FMapProperty>()) 
		{
			auto MapProperty = CastField<FMapProperty>(*It);
			FString KeyName;
			FString ValueName;
			if (SplitMapInfo(MapProperty, KeyName, ValueName))
			{
				auto Children = Root.children(*PropertyName);
				SerailizeNodeToMap(MapProperty, "", Address, Children);
			}
		}
	}
}

void UXmlSerializer::SerailizeArray(FArrayProperty* ArrayProperty, void* Address, UXmlDocument* Document)
{
	FString PropertyName = ArrayProperty->GetDisplayNameText().ToString().ToLower();

	if (GetSerializeNames().Contains(PropertyName))
	{
		PropertyName = GetSerializeNames()[PropertyName];
	}
	else
	{
		PropertyName.EndsWith("s") ? PropertyName.RemoveAt(PropertyName.Len() - 1) : PropertyName;
	}

	auto ChildItems = Document->Get().first_child().children(*PropertyName);
	SerailizeNodeToArray(ArrayProperty, Address, ChildItems);
}

void UXmlSerializer::SerailizeMap(FMapProperty* MapProperty, void* Address, UXmlDocument* Document)
{
	FString PropertyName = MapProperty->GetDisplayNameText().ToString().ToLower();
	PropertyName.RemoveSpacesInline();
	FString KeyPropertyName;
	FString ValuePropertyName;
	if (SplitMapInfo(MapProperty, KeyPropertyName, ValuePropertyName))
	{
		auto ChildItems = Document->Get().first_child().children(*ValuePropertyName);
		SerailizeNodeToMap(MapProperty, KeyPropertyName, Address, ChildItems);
	}
}

#endif

DEFINE_FUNCTION(UXmlSerializer::execBlueprintSerailizeTo)
{
	P_GET_OBJECT(UClass, SerializeClass);
	P_GET_OBJECT(UXmlDocument, Document);

	Stack.StepCompiledIn<FProperty>(NULL);
	FProperty* ValueProperty = Stack.MostRecentProperty;
	uint8* ValueAddress = Stack.MostRecentPropertyAddress;

	P_FINISH;

	P_NATIVE_BEGIN;

	ValueProperty->ClearValue(ValueAddress);

	if (SerializeClass) {
		auto Serialize = NewObject<UXmlSerializer>(GetTransientPackage(), SerializeClass, NAME_None, RF_Transient);
#if WITH_EDITOR
		if (ValueProperty->IsA<FStructProperty>()) {
			//结构体类型
			auto StructProperty = CastField<FStructProperty>(ValueProperty);
			Serialize->SerailizeStruct(StructProperty->Struct, ValueAddress, Document);
		}
		else if (ValueProperty->IsA<FArrayProperty>()) {
			//结构体数组
			FArrayProperty* ArrayProperty = CastField<FArrayProperty>(ValueProperty);
			Serialize->SerailizeArray(ArrayProperty, ValueAddress, Document);
		}
		else if (ValueProperty->IsA<FMapProperty>()) {
			//以基础类型为Key，以自定义结构体为Value的字典
			FMapProperty* MapProperty = CastField<FMapProperty>(ValueProperty);
			Serialize->SerailizeMap(MapProperty, ValueAddress, Document);
		}
		else if (ValueProperty->IsA<FObjectProperty>()) {
			//继承自UObject的对象
			//FObjectProperty* ObjectProperty = CastField<FObjectProperty>(ValueProperty);

		}
#endif
	}
	P_NATIVE_END;
}