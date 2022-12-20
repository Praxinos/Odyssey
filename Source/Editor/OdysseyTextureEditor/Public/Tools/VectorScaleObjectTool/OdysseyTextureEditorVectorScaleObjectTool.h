// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorScaleObjectTool/OdysseyPainterEditorVectorScaleObjectTool.h"
#include "OdysseyBlendParameters.h"

#include "OdysseyTextureEditorVectorScaleObjectTool.generated.h"

UCLASS()
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureEditorVectorScaleObjectTool : public UOdysseyPainterEditorVectorScaleObjectTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyTextureEditorVectorScaleObjectTool();

    //Constructor
    UOdysseyTextureEditorVectorScaleObjectTool();

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;

public:
    // Setters
    //Wether the tool can draw or not
    virtual bool CanDraw() override;

private:
    class UOdysseyTextureLayerImageVector* GetLayer() const;

private:
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

};
