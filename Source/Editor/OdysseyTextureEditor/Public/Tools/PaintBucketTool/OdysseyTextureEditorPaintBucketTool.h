// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"

#include "OdysseyTextureEditorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureEditorPaintBucketTool : public UOdysseyPainterEditorPaintBucketTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyTextureEditorPaintBucketTool();

    //Constructor
    UOdysseyTextureEditorPaintBucketTool();

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;

public:
    //Wether the tool can draw or not
    virtual bool CanDraw() override;

private:
    class UOdysseyTextureLayerImageRaster* GetLayer() const;
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);
    FOdysseyBlendParameters OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters);

protected:
    // protected Data Members
    ::ULIS::FBlock* mEditedBlock;
};
