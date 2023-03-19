// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyVector.h"

#include "OdysseyPainterEditorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorPaintBucketTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorPaintBucketTool();

    //Constructor
    UOdysseyPainterEditorPaintBucketTool();
    
    void Initialize(FOdysseyPaintEngine* iPaintEngine);

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;

    bool OnMouseDownVector( UOdysseyTextureLayerImageVector& currentRasterLayer
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    bool OnMouseDownRaster( UOdysseyTextureLayerImageRaster& currentRasterLayer
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;

    void OnMouseDragVector( UOdysseyTextureLayerImageVector& currentVectorLayer
                          , const FOdysseyPoint& iPointInTexture );
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;

    bool OnMouseUpVector( UOdysseyTextureLayerImageVector& currentVectorLayer
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey );
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;


    virtual void Commit() override;

public:
    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    uint8 Tolerance;

    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    bool Gradient;

    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    FColor Color1;

    UPROPERTY(EditAnywhere,Category="Odyssey BucketFill Tool")
    FColor Color2;

public:
    // Setters
    virtual bool CanDraw();

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
