// Copyright (C) Stormtide Ltd. 2019. All Rights Reserved.

#include "TextureExporterPNG.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

///////////////////////
///// Constructor /////
///////////////////////

UTextureExporterPNG::UTextureExporterPNG(const FObjectInitializer& OI)
	: Super(OI)
{
	SupportedClass = UTexture2D::StaticClass();
	PreferredFormatIndex = 0;
	FormatExtension.Add(TEXT("PNG"));
	FormatDescription.Add(TEXT("PNG"));
}

/////////////////////
///// Utilities /////
/////////////////////

bool CanUncompressPNG(const FTextureSource& SourceData)
{
	if (SourceData.IsValid())
	{
		const ETextureSourceFormat SourceFormat = SourceData.GetFormat();
		const bool bSupportedFormat = (SourceFormat == ETextureSourceFormat::TSF_G8 || SourceFormat == ETextureSourceFormat::TSF_RGBA8 || SourceFormat == ETextureSourceFormat::TSF_BGRA8 || SourceFormat == ETextureSourceFormat::TSF_RGBA16);

		return bSupportedFormat
			&& SourceData.GetNumMips() == 1
			&& SourceData.GetNumSlices() == 1
			&& SourceData.GetSizeX() > 0
			&& SourceData.GetSizeY() > 0;
	}

	return false;
}

//////////////////////////////
///// Exporter Overrides /////
//////////////////////////////

bool UTextureExporterPNG::SupportsObject(UObject* Object) const
{
	if (Super::SupportsObject(Object))
	{
		const UTexture2D* Texture = Cast<UTexture2D>(Object);
		return Texture && CanUncompressPNG(Texture->Source);
	}

	return false;
}

bool UTextureExporterPNG::ExportBinary(UObject* Object, const TCHAR* Type, FArchive& Ar, FFeedbackContext* Warn, int32 FileIndex /*= 0*/, uint32 PortFlags /*= 0*/)
{
	UTexture2D* Texture = CastChecked<UTexture2D>(Object);
	if (!CanUncompressPNG(Texture->Source))
	{
		return false;
	}

	const ETextureSourceFormat SourceFormat = Texture->Source.GetFormat();
	const ERGBFormat ExportFormat = SourceFormat == ETextureSourceFormat::TSF_G8 ? ERGBFormat::Gray : ERGBFormat::BGRA;
	const int32 ExportBitDepth = SourceFormat == ETextureSourceFormat::TSF_RGBA16 ? 16 : 8;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
	TSharedPtr<IImageWrapper> PNGImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	TArray64<uint8> RawData;
	Texture->Source.GetMipData(RawData, 0);

	if (PNGImageWrapper.IsValid() && PNGImageWrapper->SetRaw(RawData.GetData(), RawData.GetAllocatedSize(), Texture->Source.GetSizeX(), Texture->Source.GetSizeY(), ExportFormat, ExportBitDepth))
	{
		const TArray64<uint8>& PNGData = PNGImageWrapper->GetCompressed(100);
		if (PNGData.Num() != 0)
		{
			Ar.Serialize((void*)PNGData.GetData(), PNGData.GetAllocatedSize());
			return true;
		}
	}

	return false;
}