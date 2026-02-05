// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImportTexturesParameters.h"

class UOdysseyTextureLayerImageRaster;
class UTexture2D;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorTextureImport
{
public:
    static TArray<UOdysseyTextureLayerImageRaster*> ImportTextures(UTexture2D* DestinationTexture, const FOdysseyImportTexturesParameters& iImportData);
};
