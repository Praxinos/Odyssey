// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "OdysseyBlendParameters.h"

#include "OdysseyTextureEditorVectorGridTool.generated.h"

UCLASS()
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureEditorVectorGridTool : public UOdysseyPainterEditorVectorGridTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyTextureEditorVectorGridTool();

    //Constructor
    UOdysseyTextureEditorVectorGridTool();

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;
    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture,const FKey& iKey ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    void PropertyChanged( const FName& iPropertyName );

private:
    void Load();
    void Unload();

private:
    //Listeners
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

};
