// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"

#include "OdysseyAnimationEditorColorPickerTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationEditorColorPickerTool : public UOdysseyPainterEditorColorPickerTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyAnimationEditorColorPickerTool();

    //Constructor
    UOdysseyAnimationEditorColorPickerTool();

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

private:
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);
    FOdysseyBlendParameters OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters);

};
