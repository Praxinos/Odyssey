// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyPainterEditorRasterPaintBucketTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyImageRenderingAbility;
class FOdysseyPainterEditorRasterPaintBucketToolSourceProvider;
class UOdysseyLayer;

UENUM()
enum class EOdysseyRasterPaintBucketToolColorToleranceSource : uint8
{
    Color,
    Transparency,
    ColorAndTransparency,
    Luminosity
};

UENUM()
enum class EOdysseyRasterPaintBucketToolSource : uint8
{
    Custom UMETA(Hidden),
    CurrentLayer,
    ForegroundLayers,
    BackgroundLayers,
    AllLayers
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterPaintBucketTool : public UOdysseyPainterEditorRasterBaseTool
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
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void ExtendContextMenu(FMenuBuilder& iBuilder, const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    virtual EMouseCursor::Type GetMouseCursor() const override;

    virtual void Commit() override;

    virtual FText GetTooltip() const override;

    virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

public:
    void SetSourceProvider(TSharedPtr<FOdysseyPainterEditorRasterPaintBucketToolSourceProvider> iProvider);
    FOdysseyBlendParameters GetBlendParameters() const;

private:
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlock(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const;
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlockFromColor(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor, bool iUseColor, bool iUseTransparency) const;
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlockFromLuminosity(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const;
    void ConvertMaskBlockToColorBlock(TSharedPtr<::ULIS::FBlock> iMask, TSharedPtr<::ULIS::FBlock> iColorBlock, const ::ULIS::FColor& iColor) const;
    TSharedPtr<::ULIS::FBlock> GetSourceBlock() const;
    void IncludeColorsToMaskBlock(TSharedPtr<::ULIS::FBlock> iSrcBlock, TSharedPtr<::ULIS::FBlock> iMaskBlock) const;

    void IncludeColor( ::ULIS::FColor iColor );

    void OnRasterSelectionChanged();

    TSharedPtr<::ULIS::FBlock> GetCurrentLayerBlock() const;
    TSharedPtr<::ULIS::FBlock> GetForegroundLayersBlock() const;
    TSharedPtr<::ULIS::FBlock> GetBackgroundLayersBlock() const;
    TSharedPtr<::ULIS::FBlock> GetAllLayersBlock() const;
    TArray<FOdysseyImageRenderingAbility*> GetBackgroundLayersToExclude(UOdysseyLayer* iLayer) const;
    TArray<FOdysseyImageRenderingAbility*> GetForegroundLayersToExclude(UOdysseyLayer* iLayer) const;

public:
    UPROPERTY( EditAnywhere, Category=RasterPaintBucketTool)
    EOdysseyRasterPaintBucketToolSource Source = EOdysseyRasterPaintBucketToolSource::AllLayers;
    UPROPERTY( EditAnywhere, Category=RasterPaintBucketTool)
    EOdysseyRasterPaintBucketToolColorToleranceSource ColorToleranceSource = EOdysseyRasterPaintBucketToolColorToleranceSource::ColorAndTransparency;
    UPROPERTY( EditAnywhere, Category=RasterPaintBucketTool, meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent") )
    float ColorTolerance = 5; //0 - 100%
    UPROPERTY( EditAnywhere, Category=RasterPaintBucketTool, meta=(LinearDeltaSensitivity=1) )
    float Expansion = 2; //pixels positive and negative
    UPROPERTY( EditAnywhere, Category=RasterPaintBucketTool, meta=(ClampMin=0, UIMin=0, LinearDeltaSensitivity=1) )
    float GapTolerance = 2; //pixels positive only
    UPROPERTY( EditAnywhere, Category=RasterPaintBucketTool)
    TArray<FLinearColor> IncludeColors;

    UPROPERTY(EditInstanceOnly, Category="Blending", meta=(ShowOnlyInnerProperties))
    FOdysseyBlendParameters BlendParameters;

private:
    TSharedPtr<FOdysseyPainterEditorRasterPaintBucketToolSourceProvider> mSourceProvider;

protected:
    FOdysseyPaintEngine mPaintEngine;
};
