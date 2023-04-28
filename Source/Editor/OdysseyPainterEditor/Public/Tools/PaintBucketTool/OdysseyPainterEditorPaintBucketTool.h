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
    void Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    // Raster Mouse Down
    bool OnMouseDown( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    // Vector Mouse Down
    bool OnMouseDown( FOdysseyVectorEngine* iEngine
                    , FOdysseyVectorScene* iScene
                    , FOdysseyVectorUndo** iUndo
                    , const FOdysseyPoint& iPointInTexture
                    , const FKey& iKey );
    void OnMouseHover( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
    void OnMouseDrag( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
    bool OnMouseUp( FOdysseyVectorEngine* iEngine
                  , FOdysseyVectorScene* iScene
                  , FOdysseyVectorUndo** iUndo
                  , const FOdysseyPoint& iPointInTexture
                  , const FKey& iKey );

    virtual void Commit() override;

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
    FOdysseyVectorHandleBucket* mPickedBucketHandle;
    FOdysseyVectorBucket* mPickedBucket;
    FOdysseyVectorObject* mPickedObject;
    double mOldLocalMouseX;
    double mOldLocalMouseY;
    double mDownMouseX;
    double mDownMouseY;
    FOdysseyVectorHUDBucket mBucketHUD;
};
