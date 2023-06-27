#include "XmlBlueprintFunctionLibrary.h"
#include "PugiXmlReader.h"

bool UXmlBlueprintFunctionLibrary::IsValid_XmlNode(const FXmlNode& Node)
{
	return !Node.Node.empty();
}

FString UXmlBlueprintFunctionLibrary::Attribute_String(const FXmlNode& Node, FString Attribute, FString Default)
{
	return Attribute.IsEmpty() ? Default : Node.Node.attribute(*Attribute).as_string(*Default);
}

FName UXmlBlueprintFunctionLibrary::Attribute_Name(const FXmlNode& Node, FString Attribute, FName Default)
{
	return Attribute.IsEmpty() ? Default : Node.Node.attribute(*Attribute).as_string(*Default.ToString());
}

bool UXmlBlueprintFunctionLibrary::Attribute_Bool(const FXmlNode& Node, FString Attribute, bool Default)
{
	return Attribute.IsEmpty() ? Default : Node.Node.attribute(*Attribute).as_bool(Default);
}

float UXmlBlueprintFunctionLibrary::Attribute_Float(const FXmlNode& Node, FString Attribute, float Default)
{
	return Attribute.IsEmpty() ? Default : Node.Node.attribute(*Attribute).as_float(Default);
}

int32 UXmlBlueprintFunctionLibrary::Attribute_Int(const FXmlNode& Node, FString Attribute, int32 Default)
{
	return Attribute.IsEmpty() ? Default : Node.Node.attribute(*Attribute).as_int(Default);
}


bool UXmlBlueprintFunctionLibrary::HasAttribute(const FXmlNode& Node, FString Attribute)
{
	return Attribute.IsEmpty() ? false : !Node.Node.attribute(*Attribute).empty();
}


TArray<FXmlNode> UXmlBlueprintFunctionLibrary::NodeChildren(const FXmlNode& Node, FString XPath)
{
	TArray<FXmlNode> Ret;

	if (XPath.IsEmpty())return Ret;

	auto Result = Node.Node.select_nodes(*XPath);
	for (auto& N : Result)
	{
		Ret.Add(FXmlNode{ N.node() });
	}
	return MoveTemp(Ret);
}


FXmlNode UXmlBlueprintFunctionLibrary::NodeChild(const FXmlNode& Node, FString XPath)
{
	if (XPath.IsEmpty() || Node.Node.empty())return FXmlNode();

	auto Result = Node.Node.select_node(*XPath);
	return FXmlNode{ Result.node() };
}

FString UXmlBlueprintFunctionLibrary::NodeText(const FXmlNode& Node)
{
	return Node.Node.text().as_string();
}

FVector UXmlBlueprintFunctionLibrary::ReadVector(const FXmlNode& Node, FString Attribute)
{
	return pugi::reader::ReadVector(Node.Node, FVector::ZeroVector, *Attribute);
}


FRotator UXmlBlueprintFunctionLibrary::ReadRotator(const FXmlNode& Node, FString Attribute)
{
	return pugi::reader::ReadRotator(Node.Node, FRotator::ZeroRotator, *Attribute);
}