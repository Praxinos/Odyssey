// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"

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
    //Wether the tool can draw or not
    virtual bool CanDraw() override;

private:
    class UOdysseyTextureLayerImageRaster* GetLayer() const;
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    FOdysseyBlendParameters OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters);

protected:
    // protected Data Members
    FOdysseyTextureEditor* mEditor;
};
