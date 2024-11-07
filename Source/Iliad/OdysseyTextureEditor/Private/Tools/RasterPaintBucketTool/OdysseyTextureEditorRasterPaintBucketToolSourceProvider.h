// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketToolSourceProvider.h"

class FOdysseyTextureEditorExtension;
class FOdysseyImageRenderingAbility;
class UOdysseyLayer;
class FOdysseyTextureEditorExtension;

class FOdysseyTextureEditorRasterPaintBucketToolSourceProvider
    : public FOdysseyPainterEditorRasterPaintBucketToolSourceProvider
{
public:
    virtual ~FOdysseyTextureEditorRasterPaintBucketToolSourceProvider();
    FOdysseyTextureEditorRasterPaintBucketToolSourceProvider(FOdysseyTextureEditorExtension* iExtension);

public:
    virtual const TArray<FString>& GetSources() const override;
    virtual TSharedPtr<::ULIS::FBlock> GetSourceBlock(EOdysseyRasterPaintBucketToolSource iSource) const override;

private:
    TSharedPtr<::ULIS::FBlock> GetCurrentLayerBlock() const;
    TSharedPtr<::ULIS::FBlock> GetForegroundLayersBlock() const;
    TSharedPtr<::ULIS::FBlock> GetBackgroundLayersBlock() const;
    TSharedPtr<::ULIS::FBlock> GetAllLayersBlock() const;

    TArray<FOdysseyImageRenderingAbility*> GetForegroundLayersToExclude(UOdysseyLayer* iLayer) const;
    TArray<FOdysseyImageRenderingAbility*> GetBackgroundLayersToExclude(UOdysseyLayer* iLayer) const;

private:
    FOdysseyTextureEditorExtension* mExtension;
    TArray<FString> mSources;
};
