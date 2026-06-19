// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimation.h"
#include "OdysseyImportTexturesParameters.h"

#include "OdysseyPainterEditorAnimationImport.generated.h"

class UOdysseyAnimationCellImageRaster;
class UOdysseyAnimationLayer;
class UOdysseyAnimationLayerImageRaster;
class UTexture2D;

USTRUCT(BlueprintType)
struct FOdysseyPainterEditorAnimationImportResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    TArray<UOdysseyAnimationLayerImageRaster*> mImportedLayers;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    TArray<UOdysseyAnimationCellImageRaster*> mImportedCells;
};

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationImport
{
public:
    static FOdysseyPainterEditorAnimationImportResult ImportTextureSequence(UOdysseyAnimation* Animation, const FOdysseyImportTexturesParameters& iImportData);
};

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationLayerImport
{
public:
    static FOdysseyPainterEditorAnimationImportResult ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, const FOdysseyImportTexturesParameters& iImportData, int iCellIndex = -1);
};
