// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyTextureEditorRasterDrawingTool.generated.h"

class FOdysseyTextureEditor;

UCLASS()
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureEditorRasterDrawingTool : public UOdysseyPainterEditorRasterDrawingTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyTextureEditorRasterDrawingTool();

    //Constructor
    UOdysseyTextureEditorRasterDrawingTool();

public:
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;

public:
    // Paint Engine Stroke API

    //Wether the tool can draw or not
    virtual bool CanDraw() override;

private:
    class UOdysseyTextureLayerImageRaster* GetLayer() const;
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    // void OnLayerRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects);
    // void OnPaintEngineCommit(const TArray<::ULIS::FRectI>& iChangedTiles);
    FOdysseyBlendParameters OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters);

    //static void OnEditedBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo);

protected:
    // protected Data Members
    FOdysseyTextureEditor* mEditor;
};
