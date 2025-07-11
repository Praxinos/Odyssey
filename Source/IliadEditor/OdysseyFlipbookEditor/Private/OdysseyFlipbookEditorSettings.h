// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyFlipbookEditorSettings.generated.h"


UENUM()
enum class EOdysseyDefaultFlipbookEditor
{
    OdysseyPainterEditor UMETA( DisplayName = "Odyssey Flipbook Editor" ),
    UnrealDefaultEditor UMETA( DisplayName = "Default Flipbook Editor" )
};

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class UOdysseyFlipbookEditorSettings
    : public UObject
{
    GENERATED_BODY()

public:
    static UOdysseyFlipbookEditorSettings* Get();

public:
    /** If checked, set Odyssey as default editor when double-click on a FlipBook. If unchecked, removes the Flipbook creation icon from the Odyssey menu */
    UPROPERTY(config, EditAnywhere, Category=Defaults, DisplayName = "Flipbook Editor to use when opening a Flipbook Asset")
    EOdysseyDefaultFlipbookEditor DefaultFlipbookEditor = EOdysseyDefaultFlipbookEditor::OdysseyPainterEditor;
};
