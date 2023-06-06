// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyAnimationEditorRasterDrawingTool.generated.h"

class FOdysseyAnimationEditor;

UCLASS()
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationEditorRasterDrawingTool : public UOdysseyPainterEditorRasterDrawingTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyAnimationEditorRasterDrawingTool();

    //Constructor
    UOdysseyAnimationEditorRasterDrawingTool();

public:
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;

public:
    // Paint Engine Stroke API

    //Wether the tool can draw or not
    virtual bool CanDraw() override;

private:
    class UOdysseyAnimation* GetAnimation() const;
    class UOdysseyAnimationLayerImageRaster* GetLayer() const;
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    FOdysseyBlendParameters OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters);

protected:
    // protected Data Members
    FOdysseyAnimationEditor* mEditor;
};
