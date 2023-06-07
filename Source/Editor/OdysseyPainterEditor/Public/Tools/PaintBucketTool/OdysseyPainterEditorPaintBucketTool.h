// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyPainterEditorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorPaintBucketTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    static bool DoubleClicked();

    // Destructor
    virtual ~UOdysseyPainterEditorPaintBucketTool();

    //Constructor
    UOdysseyPainterEditorPaintBucketTool();
    
    void Initialize(FOdysseyPaintEngine* iPaintEngine);

    //OdysseyPainterEditorTool overrides
    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    // Raster Mouse Down
    bool OnMouseDownRaster( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    // Vector Mouse Down
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

    virtual void Commit() override;

    private:
        void SetBucketColor( FOdysseyVectorBucket* iBucket );

    protected:
        void OnMouseUpVectorClearBucket( FOdysseyVectorScene* iScene
                                       , FOdysseyVectorBucket* iBucket );
        void OnMouseUpVectorCreateBucket( FOdysseyVectorScene* iScene
                                        , FOdysseyVectorGroupPaint* paintGroup
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey );
        void OnMouseUpVectorRemoveBucket( FOdysseyVectorScene* iScene
                                        , FOdysseyVectorGroupPaint* paintGroup
                                        , FOdysseyVectorBucket* iBucket
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey );
        void OnMouseUpVectorMovePoint( FOdysseyVectorScene* iScene
                                     , FOdysseyVectorGroupPaint* paintGroup
                                     , FOdysseyVectorPoint* iPoint
                                     , ::ULIS::FVec2D iPointOriginalPosition
                                     , const FOdysseyPoint& iPointInTexture
                                     , const FKey& iKey );
        void OnMouseUpVectorPropagateBucket( FOdysseyVectorScene* iScene
                                           , FOdysseyVectorBucket* iBucket
                                           , bool iPropagate );
        void OnMouseUpVectorColorBucket( FOdysseyVectorScene* iScene
                                       , FOdysseyVectorBucket* iBucket );

public:
    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    uint8 Tolerance;

    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    bool Propagate;

    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    bool Gradient;

    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    FColor Color1;

    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    FColor Color2;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine mPaintEngine;
    FOdysseyVectorBucket* mPickedBucket;
    FOdysseyVectorObject* mPickedObject;
    double mOldLocalMouseX;
    double mOldLocalMouseY;
    double mDownMouseX;
    double mDownMouseY;
    ::ULIS::FVec2D mPointPosition;
    FOdysseyVectorHUDBucket mBucketHUD;
    uint32 mPickedArea;
};
