// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"

UOdysseyAnimationEditorProjectSettings::UOdysseyAnimationEditorProjectSettings()
    : AnimationCellsMarks()
{
    AnimationCellsMarks.Add(
        "red",
        {
            TEXT("Red"),
            FLinearColor::Red,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    AnimationCellsMarks.Add(
        "green",
        {
            TEXT("Green"),
            FLinearColor::Green,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    AnimationCellsMarks.Add(
        "blue",
        {
            TEXT("Blue"),
            FLinearColor::Blue,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );
}

//Static
UOdysseyAnimationEditorProjectSettings* UOdysseyAnimationEditorProjectSettings::Get()
{
    return CastChecked<UOdysseyAnimationEditorProjectSettings>(UOdysseyAnimationEditorProjectSettings::StaticClass()->GetDefaultObject());
}
