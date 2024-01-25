// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketToolSourceProvider.h"

#include "OdysseyPainterEditorRasterPaintBucketTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyPainterEditorRasterPaintBucketToolSourceProvider;

UENUM()
enum class EOdysseyRasterPaintBucketToolColorToleranceSource : uint8
{
    Color,
    Transparency,
    ColorAndTransparency,
    Luminosity
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterPaintBucketTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    DECLARE_DELEGATE_RetVal(TSharedPtr<::ULIS::FBlock>, FSourceGetter)

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
    virtual TSharedRef<SWidget> CreateTopTabWidget() override;

public:
    void SetSourceProvider(TSharedPtr<FOdysseyPainterEditorRasterPaintBucketToolSourceProvider> iProvider);
    FOdysseyBlendParameters GetBlendParameters() const;

private:
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlock(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const;
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlockFromColor(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor, bool iUseColor, bool iUseTransparency) const;
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlockFromLuminosity(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const;
    void ConvertMaskBlockToColorBlock(TSharedPtr<::ULIS::FBlock> iMask, TSharedPtr<::ULIS::FBlock> iColorBlock, const ::ULIS::FColor& iColor) const;
    TSharedPtr<::ULIS::FBlock> GetSourceBlock() const;

public:
    UPROPERTY( EditAnywhere, Category = RasterPaintBucketTool)
    EOdysseyRasterPaintBucketToolSource Source = EOdysseyRasterPaintBucketToolSource::CurrentLayer;
    UPROPERTY( EditAnywhere, Category = RasterPaintBucketTool)
    EOdysseyRasterPaintBucketToolColorToleranceSource ColorToleranceSource;
    UPROPERTY( EditAnywhere, Category = RasterPaintBucketTool, meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent") )
    float ColorTolerance; //0 - 100%
    UPROPERTY( EditAnywhere, Category = RasterPaintBucketTool, meta=(LinearDeltaSensitivity=1) )
    float Expansion; //pixels positive and negative
    UPROPERTY( EditAnywhere, Category = RasterPaintBucketTool, meta=(ClampMin=0, UIMin=0, LinearDeltaSensitivity=1) )
    float GapTolerance; //pixels positive only
    
    UPROPERTY(EditInstanceOnly, Category="Blending", meta=(ShowOnlyInnerProperties))
    FOdysseyBlendParameters BlendParameters;

private:
    TSharedPtr<FOdysseyPainterEditorRasterPaintBucketToolSourceProvider> mSourceProvider;

protected:
    FOdysseyPaintEngine mPaintEngine;
};
