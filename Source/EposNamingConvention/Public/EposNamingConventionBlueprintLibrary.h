// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "EposNamingConventionBlueprintLibrary.generated.h"

class ISequencer;
//class UEposMovieSceneSequence;
class UMovieSceneSequence;
class UNamingConventionSettings;

UCLASS()
class EPOSNAMINGCONVENTION_API UEposNamingConventionBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /*
     * Open a board sequence asset
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Naming Convention")
    static FString GenerateNoteAssetPathName( FString& Path, FString& Name );

public:

    /**
     * Get Settings
     */
    UFUNCTION( BlueprintCallable, Category = "Epos Naming Convention" )
    static UNamingConventionSettings* GetEposNamingConventionDefaultSettings();

public:

    /**
     * Internal function to assign a sequencer singleton.
     * NOTE: Only to be called by EposSequenceEditor::Construct.
     */
    static void SetSequencer(TSharedRef<ISequencer> InSequencer);
};
