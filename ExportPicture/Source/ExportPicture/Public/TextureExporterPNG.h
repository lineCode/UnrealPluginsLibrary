// Copyright (C) Stormtide Ltd. 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Exporters/Exporter.h"
#include "TextureExporterPNG.generated.h"

// Declarations
struct FTextureSource;

/*
* Texture Exporter for PNG Files
*/
UCLASS()
class UTextureExporterPNG : public UExporter
{
	GENERATED_BODY()
public:
	UTextureExporterPNG(const FObjectInitializer& OI);

	virtual bool SupportsObject(UObject* Object) const override;
	virtual bool ExportBinary(UObject* Object, const TCHAR* Type, FArchive& Ar, FFeedbackContext* Warn, int32 FileIndex = 0, uint32 PortFlags = 0) override;
};