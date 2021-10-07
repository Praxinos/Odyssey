// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyTexture2DEditorSettings.generated.h"

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class ODYSSEYTEXTURE2DEDITOR_API UOdysseyTexture2DEditorSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    static UOdysseyTexture2DEditorSettings* Get();

public:
    /** If true, set Iliad as default editor when double-click on a Texture. */
    UPROPERTY(config, EditAnywhere, Category = Defaults, DisplayName = "Set Iliad as default editor for textures")
    bool IliadDefaultEditorEnabled;
};

