// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SScrollBox.h"

#include "OdysseyPalette.generated.h"


USTRUCT(BlueprintType)
struct FOdysseyColorEntry
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FName EntryName;

    UPROPERTY()
    FColor Color;
};

/////////////////////////////////////////////////////
// OdysseyColorPalette
UCLASS()
class ODYSSEYPALETTE_API UOdysseyPalette : public UObject
{
    GENERATED_BODY()

public:
    TSharedPtr<SWidget> CreateWidget();

public:
    UPROPERTY(EditAnywhere)
    TMap<FName, FColor> mPaletteMap;

private:
    TSharedPtr<SScrollBox> mElementsWidget;
    /** The widget representation of the Bezier in Editor */
    TSharedPtr<IDetailsView> mDetailsView;
};
