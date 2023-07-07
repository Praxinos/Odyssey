// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoErase.h"

#include "OdysseyPainterEditorVectorEraserTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorEraserTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Eraser Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double Radius;

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorEraserTool();

    //Constructor
    UOdysseyPainterEditorVectorEraserTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;
    
    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
 
    //OdysseyPainterEditorTool overrides
    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine
                       , FOdysseyVectorScene* iScene );

    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey);
    void OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                           , FOdysseyVectorScene* iScene
                           , const FOdysseyPoint& iPointInTexture );
    void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture );
    bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey);
    virtual void Commit() override;

protected:
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    void PropertyChanged( const FName& iPropertyName );

private:
    FOdysseyVectorHUDEraser mEraserHUD;

};
