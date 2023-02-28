// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"
#include "OdysseyBlendParameters.h"

#include "OdysseyAnimationEditorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationEditorPaintBucketTool : public UOdysseyPainterEditorPaintBucketTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyAnimationEditorPaintBucketTool();

    //Constructor
    UOdysseyAnimationEditorPaintBucketTool();

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;

public:
    // Setters
    //Wether the tool can draw or not
    virtual bool CanDraw() override;

private:
    class UOdysseyAnimationLayerImageRaster* GetLayer() const;

private:
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);
    void OnLayerRenderImageChanged(class UOdysseyAnimationLayer* iLayer, const TArray<::ULIS::FRectI>& iRects);
    void OnPaintEngineCommit(const TArray<::ULIS::FRectI>& iChangedTiles);
    FOdysseyBlendParameters OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters);

    static void OnEditedBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo);

protected:
    // protected Data Members
    ::ULIS::FBlock* mEditedBlock;
};
