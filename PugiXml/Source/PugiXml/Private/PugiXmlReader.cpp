// Fill out your copyright notice in the Description page of Project Settings.

#include "PugiXmlReader.h"


#define LOCTEXT_NAMESPACE "FPugiXml"


namespace pugi
{
	namespace reader
	{
		static TArray<FString> StringToArray(const FString& Str)
		{
			TArray<FString> Arr;
			Str.ParseIntoArray(Arr, L" ");
			return MoveTemp(Arr);
		}

		static TArray<FString> ReadStringArray(const pugi::xml_node& InNode, const pugi::char_t* AttributeName)
		{
			FString Str = InNode.attribute(AttributeName).as_string();
			return StringToArray(Str);
		}

		FMargin ReadPadding(const pugi::xml_node& InNode, const FMargin& Default, const pugi::char_t* AttributeName)
		{
			auto Arr = ReadStringArray(InNode, AttributeName);
			switch (Arr.Num())
			{
			case 1: return FMargin(FCString::Atof(*Arr[0]));
			case 2: return FMargin(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]));
			case 3: return FMargin(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]), FCString::Atof(*Arr[2]), 0.0f);
			case 4: return FMargin(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]), FCString::Atof(*Arr[2]), FCString::Atof(*Arr[3]));
			}

			return Default;
		}


		FVector2D ReadVector2D(const pugi::xml_node& InNode, const FVector2D& Default, const pugi::char_t* AttributeName)
		{
			auto Arr = ReadStringArray(InNode, AttributeName);
			switch (Arr.Num())
			{
			case 1: return FVector2D(FCString::Atof(*Arr[0]));
			case 2: return FVector2D(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]));
			}

			return Default;
		}

		FVector ReadVector(const pugi::xml_node& InNode, const FVector& Default, const pugi::char_t* AttributeName)
		{
			FString Str = InNode.attribute(AttributeName).as_string();
			FVector Vec;
			if (Vec.InitFromString(Str)) {
				return Vec;
			}

			auto Arr = StringToArray(Str);
			switch (Arr.Num())
			{
			case 1: return FVector(FCString::Atof(*Arr[0]), 0.0f, 0.0f);
			case 2: return FVector(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]), 0.0f);
			case 3: return FVector(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]), FCString::Atof(*Arr[2]));
			}
			return Default;
		}

		FRotator ReadRotator(const pugi::xml_node& InNode, const FRotator& Default, const pugi::char_t* AttributeName)
		{
			FString Str = InNode.attribute(AttributeName).as_string();
			FRotator Rot;
			if (FParse::Value(*Str, TEXT("Pitch="), Rot.Pitch) && FParse::Value(*Str, TEXT("Yaw="), Rot.Yaw) && FParse::Value(*Str, TEXT("Roll="), Rot.Roll)) {
				return Rot;
			}

			auto Arr = StringToArray(Str);
			switch (Arr.Num())
			{
			case 1: return FRotator(0.0f, 0.0f, FCString::Atof(*Arr[0]));
			case 2: return FRotator(FCString::Atof(*Arr[1]), 0.0f, FCString::Atof(*Arr[0]));
			case 3: return FRotator(FCString::Atof(*Arr[1]), FCString::Atof(*Arr[2]), FCString::Atof(*Arr[0]));
			}
			return Default;
		}

		FAnchors ReadAnchors(const pugi::xml_node& InNode, const FAnchors& Default, const pugi::char_t* AttributeName)
		{
			auto Arr = ReadStringArray(InNode, AttributeName);
			switch (Arr.Num())
			{
			case 1: return FAnchors(FCString::Atof(*Arr[0]));
			case 2: return FAnchors(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]));
			case 3: return FAnchors(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]), FCString::Atof(*Arr[2]), 0.0f);
			case 4: return FAnchors(FCString::Atof(*Arr[0]), FCString::Atof(*Arr[1]), FCString::Atof(*Arr[2]), FCString::Atof(*Arr[3]));
			}
			return Default;
		}
	}
}

#undef LOCTEXT_NAMESPACE