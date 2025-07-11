// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyTextureEditorSettings.generated.h"

UENUM()
enum class EOdysseyDefaultTextureEditor
{
    OdysseyPainterEditor UMETA( DisplayName = "Odyssey Texture Editor" ),
    UnrealDefaultEditor UMETA( DisplayName = "Default Texture Editor" )
};

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class UOdysseyTextureEditorSettings
    : public UObject
{
    GENERATED_BODY()

public:
    static UOdysseyTextureEditorSettings* Get();

public:
    /** If true, set Odyssey as default editor when double-click on a Texture. */
    UPROPERTY(config, EditAnywhere, Category=Defaults, DisplayName = "Texture Editor to use when opening a Texture Asset")
    EOdysseyDefaultTextureEditor DefaultTextureEditor = EOdysseyDefaultTextureEditor::OdysseyPainterEditor;
};
