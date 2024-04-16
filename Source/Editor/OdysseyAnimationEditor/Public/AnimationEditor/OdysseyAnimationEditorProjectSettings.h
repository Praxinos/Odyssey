// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyAnimationEditorProjectSettings.generated.h"

UENUM()
enum class EOdysseyAnimationCellMarkSymbol : uint8
{
    Fill,
    Triangle,
    Circle,
    Diamond,
    Star
};

USTRUCT()
struct FAnimationCellMarkSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Cell Marks")
    FName Name;

    UPROPERTY(EditAnywhere, Category="Cell Marks", meta=(HideAlphaChannel))
    FLinearColor Color = FLinearColor::White;

    UPROPERTY(EditAnywhere, Category="Cell Marks")
    EOdysseyAnimationCellMarkSymbol Symbol;
};

UCLASS(config=Engine)
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationEditorProjectSettings
    : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyAnimationEditorProjectSettings();
    static UOdysseyAnimationEditorProjectSettings* Get();

public:
    /** The type of background to draw in the texture editor view port. */
    UPROPERTY(config, EditAnywhere, Category = "Animation Cell Marks Settings", meta=(ForceInlineRow))
    TMap<FString, FAnimationCellMarkSettings> AnimationCellsMarks;

    UPROPERTY(config, EditAnywhere, Category = "Animation Cell Marks Settings", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent") )
    float AnimationCellsMarksFillOpacity = 10.f;
};
