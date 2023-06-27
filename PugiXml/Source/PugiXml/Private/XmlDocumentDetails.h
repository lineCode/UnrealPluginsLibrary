// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#if WITH_EDITOR

#include "IDetailCustomization.h"

class FXmlDocumentDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

#endif