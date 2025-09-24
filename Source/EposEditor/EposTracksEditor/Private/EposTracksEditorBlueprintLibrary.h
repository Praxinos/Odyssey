// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

//#include "LevelSequenceEditorBlueprintLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Settings/EposTracksEditorSettings.h"

#include "EposTracksEditorBlueprintLibrary.generated.h"

//---

UCLASS()
class EPOSTRACKSEDITOR_API UEposTracksEditorBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * Get Settings
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Storyboard Editor|Tracks" )
    static UEposTracksEditorSettings* GetEposTracksEditorDefaultSettings();
};
