// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimation.h"

class UOdysseyAnimationCellImageRaster;
class UOdysseyAnimationLayer;
class UOdysseyAnimationLayerImageRaster;
class UTexture2D;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationImport
{
public:
    FOdysseyPainterEditorAnimationImport();

public:
    UOdysseyAnimationLayerImageRaster* ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, UOdysseyAnimationLayer* ParentLayer = nullptr, int IndexInParent = 0);

    UOdysseyAnimationLayerImageRaster* ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, UOdysseyAnimationLayer* ParentLayer = nullptr, int IndexInParent = 0);
};

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationLayerImport
{
public:
    FOdysseyPainterEditorAnimationLayerImport();

public:
    TArray<UOdysseyAnimationCellImageRaster*> ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<UTexture2D*> Textures, int iCellIndex = -1);

    TArray<UOdysseyAnimationCellImageRaster*> ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, int iCellIndex = -1);
};
