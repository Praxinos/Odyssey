// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "OdysseyPainterEditorRasterPaintBucketToolSourceProvider.generated.h"

UENUM()
enum class EOdysseyRasterPaintBucketToolSource : uint8
{
    Custom UMETA(Hidden),
    CurrentLayer,
    ForegroundLayers,
    BackgroundLayers,
    AllLayers
};

class FOdysseyPainterEditorRasterPaintBucketToolSourceProvider
{
public:
    virtual const TArray<FString>& GetSources() const = 0;
    virtual TSharedPtr<::ULIS::FBlock> GetSourceBlock(EOdysseyRasterPaintBucketToolSource iSource) const = 0;
};
