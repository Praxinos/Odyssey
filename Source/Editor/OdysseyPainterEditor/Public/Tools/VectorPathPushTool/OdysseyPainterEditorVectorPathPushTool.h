// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorPathPushTool.generated.h"

class FOdysseyPainterEditorVectorPathPushToolHUD;

typedef struct _FPushedPoint
{
     FOdysseyVectorPoint* point;
     double ratio;
     bool isSmooth;
     ::ULIS::FVec2D perpendicularVector; // perpendicular vector

     _FPushedPoint( FOdysseyVectorPoint* iPoint, double iRatio, bool iIsSmooth )
     {
         point = iPoint;
         ratio = ( iRatio > 1.0f ) ? 1.0f : iRatio;
         isSmooth = iIsSmooth;

         if( isSmooth )
         {
             if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
             {
                 FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(iPoint);
                 FOdysseyVectorSegment* segment = vertex->GetFirstSegment();

                 if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                 {
                     FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                     perpendicularVector = cubicSegment->GetHandleVector( vertex, true );
                 }
             }
         }
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

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

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
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    void PropertyChanged( const FName& iPropertyName );

private:
    bool HasVertex( FOdysseyVectorPoint* iPoint );
    std::vector<FPushedPoint> mPushedPointArray;
    std::vector<FOdysseyVectorSegment*> mSegmentArray;
    //FOdysseyVectorHUDPicking mPickingHUD;
    FOdysseyPainterEditorVectorPathPushToolHUD *mPathPushHUD;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey PathPush Tool", meta = (ClampMin = "0.0", UIMin = "0.0"))
    double Radius;

    UPROPERTY(EditAnywhere, Category="Odyssey PathPush Tool")
    bool PreserveSmoothness;

    UPROPERTY(EditAnywhere, Category="Odyssey PathPush Tool")
    bool RestrictToSelection;
};
