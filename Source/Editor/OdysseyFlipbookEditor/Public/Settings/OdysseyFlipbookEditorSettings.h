// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyFlipbookEditorSettings.generated.h"

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class ODYSSEYFLIPBOOKEDITOR_API UOdysseyFlipbookEditorSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

private:
    #if WITH_EDITOR
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    #endif

public:
    static UOdysseyFlipbookEditorSettings* Get();

public:
    /** If checked, set Iliad as default editor when double-click on a FlipBook. If unchecked, removes the Flipbook creation icon from the Iliad menu */
    UPROPERTY(config, EditAnywhere, Category = Defaults, DisplayName = "Set Iliad as default editor for Flipbooks")
    bool IliadDefaultEditorEnabled;

    /* UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord PreviousFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord NextFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord PreviousKeyFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord NextKeyFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord FirstFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord LastFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord PlayForward;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord PlayBackward;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord Pause;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord Stop;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord ToggleLoopingMode;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord AddKeyFrameAtEnd;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord AddKeyFrameBeforeCurrentKeyFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord AddKeyFrameAfterCurrentKeyFrame;

    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord DuplicateCurrentKeyFrame;
    
    UPROPERTY(config, EditAnywhere, Category=Shortcuts )
    FInputChord DeleteCurrentKeyFrame; */
};
