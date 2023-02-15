// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"

#include "OdysseyTextureEditorColorPickerTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureEditorColorPickerTool : public UOdysseyPainterEditorColorPickerTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyTextureEditorColorPickerTool();

    //Constructor
    UOdysseyTextureEditorColorPickerTool();

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
