// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"

#include "OdysseyPainterEditorVectorPathEditTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathEditTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPathEditTool();

    //Constructor
    UOdysseyPainterEditorVectorPathEditTool();
 
    void Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    bool OnMouseDown( FOdysseyVectorEngine* iEngine
                    , FOdysseyVectorScene* iScene
                    , FOdysseyVectorUndo** iUndo
                    , const FOdysseyPoint& iPointInTexture
                    , const FKey& iKey );
    void OnMouseHover( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
    void OnMouseDrag( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
    bool OnMouseUp( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    bool OnKeyDown( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FKey& iKey );
    bool OnKeyUp( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FKey& iKey );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

protected:
    void PropertyChanged( const FName& iPropertyName );

    FOdysseyVectorPathCubic* FetchPath( FOdysseyVectorEngine* iVectorEngine
                                      , FOdysseyVectorScene* iScene
                                      , double iWorldX
                                      , double iWorldY );

private:
    // the "undo segment reshape" object is used in both the MouseDown and MouseUp events. We need to remember it.
    FOdysseyVectorUndoPointPosition* mUndoPointPosition;
    double mOldLocalMouseX;
    double mOldLocalMouseY;
    FOdysseyVectorHUDPathCubic mCubicPathHUD;
    FOdysseyVectorHUDPicking mPickingHUD;
    std::vector<FOdysseyVectorPoint*> mPickedPointArray;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey PathEdit Tool")
    double Radius;
};
