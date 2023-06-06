// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

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

    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    void OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                           , FOdysseyVectorScene* iScene
                           , const FOdysseyPoint& iPointInTexture );
    void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                           , const FOdysseyPoint& iPointInTexture );
    bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey );
    bool OnKeyDownVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FKey& iKey );
    bool OnKeyUpVector( FOdysseyVectorEngine* iEngine
                      , FOdysseyVectorScene* iScene
                      , const FKey& iKey );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

protected:
    void PropertyChanged( const FName& iPropertyName );

    FOdysseyVectorPathCubic* FetchPath( FOdysseyVectorEngine* iVectorEngine
                                      , FOdysseyVectorScene* iScene
                                      , double iWorldX
                                      , double iWorldY );
    void OnMouseDownDeletePoint( FOdysseyVectorEngine* iEngine
                               , FOdysseyVectorScene* iScene
                               , FOdysseyVectorPath* iPath
                               , const FOdysseyPoint& iPointInTexture
                               , const FKey& iKey );
    void OnMouseDownMovePoint( FOdysseyVectorEngine* iEngine
                             , FOdysseyVectorScene* iScene
                             , FOdysseyVectorPath* iPath
                             , const FOdysseyPoint& iPointInTexture
                             , const FKey& iKey );

private:
    double mOldLocalMouseX;
    double mOldLocalMouseY;
    FOdysseyVectorHUDPathCubic mCubicPathHUD;
    FOdysseyVectorHUDPicking mPickingHUD;
    std::vector<FOdysseyVectorPoint*> mPickedPointArray;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey PathEdit Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double Radius;
};
