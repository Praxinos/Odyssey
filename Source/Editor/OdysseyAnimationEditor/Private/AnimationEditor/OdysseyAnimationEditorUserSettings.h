// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationEditorFlipSystem.h"
#include "Engine/DeveloperSettings.h"
#include "OdysseyAnimationEditorUserSettings.generated.h"

UENUM()
enum class EOdysseyAnimationEditorUserSettingsFlipConfigurations : uint8
{
    Configuration1 UMETA(DisplayName = "#1"),
    Configuration2 UMETA(DisplayName = "#2"),
    Configuration3 UMETA(DisplayName = "#3"),
    Configuration4 UMETA(DisplayName = "#4"),
    Configuration5 UMETA(DisplayName = "#5"),
    Configuration6 UMETA(DisplayName = "#6"),
    Configuration7 UMETA(DisplayName = "#7"),
    Configuration8 UMETA(DisplayName = "#8"),
    Configuration9 UMETA(DisplayName = "#9"),
    Configuration10 UMETA(DisplayName = "#10")
};

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=Odyssey, defaultconfig)
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationEditorUserSettings
    : public UObject
{
    GENERATED_BODY()

public:
    static const UOdysseyAnimationEditorUserSettings* Get();

public:
    UPROPERTY(config, EditAnywhere, EditFixedSize, Category="Animation Flip Settings", meta=(NoElementDuplicate, EditFixedOrder, ArraySizeEnum="/Script/OdysseyAnimationEditor.EOdysseyAnimationEditorUserSettingsFlipConfigurations"))
    FOdysseyAnimationFlipConfiguration FlipConfigurations[10];

	UPROPERTY(config, EditAnywhere, Category="Animation Timeline", meta = (LinearDeltaSensitivity="1"))
	int StartFrame = 1;
};
