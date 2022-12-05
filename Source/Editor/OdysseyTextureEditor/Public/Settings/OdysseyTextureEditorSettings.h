// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyTextureEditorSettings.generated.h"

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class ODYSSEYTEXTUREEDITOR_API UOdysseyTextureEditorSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    static UOdysseyTextureEditorSettings* Get();

public:
    /** If true, set Iliad as default editor when double-click on a Texture. */
    UPROPERTY(config, EditAnywhere, Category = Defaults, DisplayName = "Set Iliad as default editor for textures")
    bool IliadDefaultEditorEnabled;
};

