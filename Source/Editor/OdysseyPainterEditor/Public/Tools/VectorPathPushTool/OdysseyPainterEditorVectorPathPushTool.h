// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoSegmentReshape.h"

#include "OdysseyPainterEditorVectorPathPushTool.generated.h"

typedef struct _FPushedPoint
{
     FOdysseyVectorPoint* point;
     double ratio;
     _FPushedPoint( FOdysseyVectorPoint* iPoint, double iRatio )
     {
         point = iPoint;
         ratio = ( iRatio > 1.0f ) ? 1.0f : iRatio;
     }
} FPushedPoint;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathPushTool : public UOdysseyPainterEditorDefaultTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPathPushTool();

    //Constructor
    UOdysseyPainterEditorVectorPathPushTool();

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

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

protected:
    void PropertyChanged( const FName& iPropertyName );

private:
    bool HasVertex( FOdysseyVectorPoint* iPoint );
    std::vector<FPushedPoint> mPushedPointArray;
    std::vector<FOdysseyVectorSegment*> mSegmentArray;
    FOdysseyVectorHUDPicking mPickingHUD;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey PathPush Tool")
    double Radius;

    UPROPERTY(EditAnywhere,Category="Odyssey PathPush Tool")
    bool PreserveSmoothness;

};
