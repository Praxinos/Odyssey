// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    UFUNCTION( BlueprintCallable, Category = "Epos Tracks Editor" )
    static UEposTracksEditorSettings* GetEposTracksEditorDefaultSettings();
};
