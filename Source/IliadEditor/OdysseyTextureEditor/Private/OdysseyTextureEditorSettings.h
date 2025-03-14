// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "OdysseyTextureEditorSettings.generated.h"

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class UOdysseyTextureEditorSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    static UOdysseyTextureEditorSettings* Get();

public:
    /** If true, set Odyssey as default editor when double-click on a Texture. */
    UPROPERTY(config, EditAnywhere, Category=Defaults, DisplayName = "Set Odyssey as default editor for textures")
    bool OdysseyDefaultEditorEnabled;
};
