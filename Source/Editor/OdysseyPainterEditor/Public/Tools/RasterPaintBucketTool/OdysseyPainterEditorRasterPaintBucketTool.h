// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyPainterEditorRasterPaintBucketTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterPaintBucketTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    //Inactivates the tool
    virtual bool IsActivable() const override;

    virtual void Load();
    virtual void Unload();


    static bool DoubleClicked();

    // Destructor
    virtual ~UOdysseyPainterEditorRasterPaintBucketTool();

    //Constructor
    UOdysseyPainterEditorRasterPaintBucketTool();
    
    void Initialize(FOdysseyPaintEngine* iPaintEngine);

    virtual bool OnKeyDown( const FKey& iKey ) override;
    virtual bool OnKeyUp( const FKey& iKey ) override;
    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    // Raster Mouse Down
    bool OnMouseDownRaster( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const FOdysseyPoint& iPointInTexture, const FKey& iKey );

    virtual void Commit() override;

public:
    UPROPERTY( EditAnywhere, Category = RasterPaintBucketTool )
    uint8 Tolerance;

protected:
    FOdysseyPaintEngine mPaintEngine;
};
