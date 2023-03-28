// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorPathWidthTool/OdysseyPainterEditorVectorPathWidthTool.h"
#include "OdysseyBlendParameters.h"

#include "OdysseyAnimationEditorVectorPathWidthTool.generated.h"

UCLASS()
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationEditorVectorPathWidthTool : public UOdysseyPainterEditorVectorPathWidthTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyAnimationEditorVectorPathWidthTool();

    //Constructor
    UOdysseyAnimationEditorVectorPathWidthTool();

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;

    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

private:
    void Load();
    void Unload();
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

private:
    //Listeners
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

};
