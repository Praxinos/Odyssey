// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"

UOdysseyAnimationEditorProjectSettings::UOdysseyAnimationEditorProjectSettings()
{
}

void
UOdysseyAnimationEditorProjectSettings::PostInitProperties()
{
    Super::PostInitProperties();

    LoadConfig();

    TArray<FAnimationCellMarkSettings> defaultCellMarks;

    defaultCellMarks.Add(
        {
            TEXT("Key 1"),
            FLinearColor::Red,
            EOdysseyAnimationCellMarkSymbol::Circle
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Key 2"),
            FLinearColor::Green,
            EOdysseyAnimationCellMarkSymbol::Circle
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Key 3"),
            FLinearColor::Blue,
            EOdysseyAnimationCellMarkSymbol::Circle
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Breakdown 1"),
            FLinearColor( 1.f, 0.f, 1.f ),
            EOdysseyAnimationCellMarkSymbol::Triangle
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Breakdown 2"),
            FLinearColor( 1.f, 1.f, 0.f ),
            EOdysseyAnimationCellMarkSymbol::Triangle
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Breakdown 3"),
            FLinearColor( 0.f, 1.f, 1.f ),
            EOdysseyAnimationCellMarkSymbol::Triangle
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Cleaned Key"),
            FLinearColor::Black,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Empty Cell"),
            FLinearColor::White,
            EOdysseyAnimationCellMarkSymbol::Fill
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("To be checked"),
            FLinearColor( 1.f, 0.3f, 0.475 ),
            EOdysseyAnimationCellMarkSymbol::Star
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("To redo"),
            FLinearColor( 1.f, 0.2f, 0.f ),
            EOdysseyAnimationCellMarkSymbol::Diamond
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Note"),
            FLinearColor( 0.3f, 0.f, 0.6f ),
            EOdysseyAnimationCellMarkSymbol::Diamond
        }
    );

    defaultCellMarks.Add(
        {
            TEXT("Validated"),
            FLinearColor( 0.f, 0.5f, 0.3f ),
            EOdysseyAnimationCellMarkSymbol::Star
        }
    );

    for (int i = AnimationCellsMarks.Num(); i < 12; i++)
    {
        AnimationCellsMarks.Add(defaultCellMarks[i]);
    }

    //Force the number of marks to 12, not more
    for (int i = AnimationCellsMarks.Num() - 1; i >= 12; i--)
    {
        AnimationCellsMarks.RemoveAt(i);
    }
}

void UOdysseyAnimationEditorProjectSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SaveConfig();
}

//Static
UOdysseyAnimationEditorProjectSettings* UOdysseyAnimationEditorProjectSettings::Get()
{
    return CastChecked<UOdysseyAnimationEditorProjectSettings>(UOdysseyAnimationEditorProjectSettings::StaticClass()->GetDefaultObject());
}
