// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyShape.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorRasterSelection.h"

#include "OdysseyMagicWandShape.generated.h"

class FOdysseyHUDRectangle;
class FOdysseyHUDHandle;
class FOdysseyPainterEditor;
class UOdysseyLayer;
class IOdysseyRenderingAbility;

UENUM()
enum class EOdysseyRasterMagicWandToolColorToleranceSource : uint8
{
    Color,
    Transparency,
    ColorAndTransparency,
    Luminosity
};

UENUM()
enum class EOdysseyRasterMagicWandToolSource : uint8
{
    Custom UMETA(Hidden),
    CurrentLayer,
    ForegroundLayers,
    BackgroundLayers,
    AllLayers
};

UCLASS(meta=(DisplayName="Magic Wand Shape"))
class UOdysseyMagicWandShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    // Destructor
    virtual ~UOdysseyMagicWandShape();

public:
    FOdysseyPainterEditor* GetEditor() const;
    void SetEditor( FOdysseyPainterEditor* iEditor );

    TSharedPtr<::ULIS::FBlock> GetSourceBlock() const;
    TSharedPtr<::ULIS::FBlock> GetCurrentLayerBlock() const;
    TSharedPtr<::ULIS::FBlock> GetForegroundLayersBlock() const;
    TSharedPtr<::ULIS::FBlock> GetBackgroundLayersBlock() const;
    TSharedPtr<::ULIS::FBlock> GetAllLayersBlock() const;

    void IncludeColorsToMaskBlock(TSharedPtr<::ULIS::FBlock> iSrcBlock, TSharedPtr<::ULIS::FBlock> iMaskBlock) const;

    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlock(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const;
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlockFromColor(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor, bool iUseColor, bool iUseTransparency) const;
    TSharedPtr<::ULIS::FBlock> CreateSourceMaskBlockFromLuminosity(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const;

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);

private:
    TArray<TArray<FOdysseyPoint>> BuildContours(const TArray<FIntEdge>& edges);
    TArray<TArray<FOdysseyPoint>> BuildContoursFromBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);
    ::ULIS::FRectI ComputeBoundingRect(const TArray<FVector2D>& iPoints) const;

    TArray<IOdysseyRenderingAbility*> GetBackgroundLayersToExclude(UOdysseyLayer* iLayer) const;
    TArray<IOdysseyRenderingAbility*> GetForegroundLayersToExclude(UOdysseyLayer* iLayer) const;

private:
    FOdysseyPainterEditor* mEditor;
    FOdysseyPaintEngine mPaintEngine;

public:
    /** The layer source(s). */
    UPROPERTY(EditAnywhere, Category = "RasterMagicWandTool")
    EOdysseyRasterMagicWandToolSource Source = EOdysseyRasterMagicWandToolSource::CurrentLayer;

    /** The type of the color tolerance used in the source. */
    UPROPERTY(EditAnywhere, Category = "RasterMagicWandTool")
    EOdysseyRasterMagicWandToolColorToleranceSource ColorToleranceSource = EOdysseyRasterMagicWandToolColorToleranceSource::ColorAndTransparency;

    /** Should we check only contiguous pixels for the magic wand or should we check the whole image: may result in multiple sub selections zones if not contiguous */
    UPROPERTY(EditAnywhere, Category = "RasterMagicWandTool")
    bool Contiguous = true;

    /** The color tolerance. */
    UPROPERTY(EditAnywhere, Category = "RasterMagicWandTool", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Units = "Percent"))
    float ColorTolerance = 25; //0 - 100%

    /** The expansion. */
    UPROPERTY(EditAnywhere, Category = "RasterMagicWandTool", meta = (LinearDeltaSensitivity = "15", Delta = "1"))
    float Expansion = 0; //pixels positive and negative

    /** The gap tolerance. */
    UPROPERTY(EditAnywhere, Category = "RasterMagicWandTool", meta = (ClampMin = "0", UIMin = "0", LinearDeltaSensitivity = "15", Delta = "1"))
    float GapTolerance = 0; //pixels positive only

    /** The list of color to include. */
    UPROPERTY(EditAnywhere, Category = "RasterMagicWandTool")
    TArray<FLinearColor> IncludeColors;
};
