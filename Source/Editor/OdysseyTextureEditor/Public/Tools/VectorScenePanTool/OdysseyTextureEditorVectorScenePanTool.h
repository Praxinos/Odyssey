// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "OdysseyBlendParameters.h"

#include "OdysseyTextureEditorVectorScenePanTool.generated.h"

UCLASS()
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureEditorVectorScenePanTool : public UOdysseyPainterEditorVectorScenePanTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyTextureEditorVectorScenePanTool();

    //Constructor
    UOdysseyTextureEditorVectorScenePanTool();

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
