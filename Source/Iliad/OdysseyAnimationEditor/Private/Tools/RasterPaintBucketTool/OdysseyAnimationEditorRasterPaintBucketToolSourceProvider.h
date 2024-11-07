// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketToolSourceProvider.h"

class FOdysseyAnimationEditorExtension;
class FOdysseyImageRenderingAbility;
class UOdysseyLayer;

class FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider
    : public FOdysseyPainterEditorRasterPaintBucketToolSourceProvider
{
public:
    virtual ~FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider();
    FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider(FOdysseyAnimationEditorExtension* iExtension);

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
    FOdysseyAnimationEditorExtension* mExtension;
    TArray<FString> mSources;
};
