// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"

UOdysseyAnimationEditorProjectSettings::UOdysseyAnimationEditorProjectSettings()
    : AnimationCellsMarks()
{
}

void
UOdysseyAnimationEditorProjectSettings::PostInitProperties()
{
    Super::PostInitProperties();

    TArray<FAnimationCellMarkSettings> defaultCellMarks;
    
    defaultCellMarks.Add(
        {
            TEXT("Red"),
            FLinearColor::Red,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Green"),
            FLinearColor::Green,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Blue"),
            FLinearColor::Blue,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Red"),
            FLinearColor::Red,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Green"),
            FLinearColor::Green,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Blue"),
            FLinearColor::Blue,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Red"),
            FLinearColor::Red,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Green"),
            FLinearColor::Green,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Blue"),
            FLinearColor::Blue,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Red"),
            FLinearColor::Red,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Green"),
            FLinearColor::Green,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Blue"),
            FLinearColor::Blue,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );   

    for (int i = AnimationCellsMarks.Num(); i < 12; i++)
    {
        AnimationCellsMarks.Add(defaultCellMarks[i]);
    }

    for (int i = AnimationCellsMarks.Num() - 1; i >= 12; i--)
    {
        AnimationCellsMarks.RemoveAt(i);
    }
}

//Static
UOdysseyAnimationEditorProjectSettings* UOdysseyAnimationEditorProjectSettings::Get()
{
    return CastChecked<UOdysseyAnimationEditorProjectSettings>(UOdysseyAnimationEditorProjectSettings::StaticClass()->GetDefaultObject());
}
