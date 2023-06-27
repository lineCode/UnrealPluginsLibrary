// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PugiXml.h"


namespace pugi
{
	namespace reader
	{
		PUGIXML_API FMargin ReadPadding(const pugi::xml_node& InNode, const FMargin& Default = FMargin(), const pugi::char_t* AttributeName = PUGIXML_TEXT("padding"));
		PUGIXML_API FVector2D ReadVector2D(const pugi::xml_node& InNode, const FVector2D& Default = FVector2D::ZeroVector, const pugi::char_t* AttributeName = PUGIXML_TEXT(""));
		PUGIXML_API FVector ReadVector(const pugi::xml_node& InNode, const FVector& Default = FVector::ZeroVector, const pugi::char_t* AttributeName = PUGIXML_TEXT("loc"));
		PUGIXML_API FRotator ReadRotator(const pugi::xml_node& InNode, const FRotator& Default = FRotator::ZeroRotator, const pugi::char_t* AttributeName = PUGIXML_TEXT("rot"));
		PUGIXML_API FAnchors ReadAnchors(const pugi::xml_node& InNode, const FAnchors& Default = FAnchors(), const pugi::char_t* AttributeName = PUGIXML_TEXT("anchors"));
	}
}
