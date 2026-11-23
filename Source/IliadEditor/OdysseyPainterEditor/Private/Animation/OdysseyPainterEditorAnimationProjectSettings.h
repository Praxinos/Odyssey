// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPainterEditorAnimationProjectSettings.generated.h"

UENUM()
enum class EOdysseyAnimationCellMarkSymbol : uint8
{
    Fill,
    Triangle,
    FilledTriangle,
    Circle,
    FilledCircle,
    Diamond,
    FilledDiamond,
    Star,
    FilledStar,
    Cross,
    Checkmark,
};

USTRUCT()
struct FAnimationCellMarkSettings
{
    GENERATED_BODY()

    /** Name of the Cell Mark displayed in the contextual menu. */
    UPROPERTY(EditAnywhere, Category="Cell Marks")
    FName Name;

    /** Color of the Cell Mark symbol. */
    UPROPERTY(EditAnywhere, Category="Cell Marks", meta=(HideAlphaChannel))
    FLinearColor Color = FLinearColor::White;

    /**
     * Type of symbol displayed for the Cell Mark.
     *
     * - Triangle, Circle, Diamond, Star (filled or outlined)
     * - Cross
     * - Checkmark
     * - Fill: simply fills the entire Cell
     */
    UPROPERTY(EditAnywhere, Category="Cell Marks")
    EOdysseyAnimationCellMarkSymbol Symbol = EOdysseyAnimationCellMarkSymbol::Fill;
};

UCLASS(config=EditorPerProjectUserSettings)
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorAnimationProjectSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyPainterEditorAnimationProjectSettings();
    static UOdysseyPainterEditorAnimationProjectSettings* Get();


    virtual void PostInitProperties() override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
    /** The type of background to draw in the texture editor view port. */
    UPROPERTY(config, EditAnywhere, EditFixedSize, Category="Animation Cell Marks Settings", meta=(ForceInlineRow, EditFixedOrder))
    TArray<FAnimationCellMarkSettings> AnimationCellsMarks;

    /** Opacity of Cell Marks that use the Fill symbol, in percent. */
    UPROPERTY(config, EditAnywhere, Category="Animation Cell Marks Settings", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent") )
    float AnimationCellsMarksFillOpacity = 75.f;
};
