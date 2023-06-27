// Fill out your copyright notice in the Description page of Project Settings.

#include "XmlDocumentDetails.h"

#if WITH_EDITOR
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Slate.h"

#define LOCTEXT_NAMESPACE "FPugiXml"

TSharedRef<IDetailCustomization> FXmlDocumentDetails::MakeInstance()
{
	return MakeShareable(new FXmlDocumentDetails());
}

void FXmlDocumentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto Content = DetailBuilder.GetProperty("Content");
	auto& Category = DetailBuilder.EditCategory("Document");

	DetailBuilder.HideProperty(Content);
	Category.AddProperty(DetailBuilder.GetProperty("SourceFile"));
	auto& Row = Category.AddCustomRow(LOCTEXT("ContentFilterKey", "Content"));
	Row.WholeRowContent()[
		SNew(SBox).MaxDesiredHeight(500.0f)[
			SNew(SScrollBox) + SScrollBox::Slot()[
				Content->CreatePropertyValueWidget()
			]
		]
	];
}




#undef LOCTEXT_NAMESPACE

#endif
	