// Copyright (C) Stormtide Ltd. 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Exporters/Exporter.h"
#include "TextureExporterJPG.generated.h"

// Declarations
struct FTextureSource;

/*
* Texture Exporter for PNG Files
*/
UCLASS()
class UTextureExporterJPG : public UExporter
{
	GENERATED_BODY()
public:
	UTextureExporterJPG(const FObjectInitializer& OI);

	virtual bool SupportsObject(UObject* Object) const override;
	virtual bool ExportBinary(UObject* Object, const TCHAR* Type, FArchive& Ar, FFeedbackContext* Warn, int32 FileIndex = 0, uint32 PortFlags = 0) override;
};