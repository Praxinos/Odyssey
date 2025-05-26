// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FMenuBuilder;

class EPOSTRACKSEDITOR_API EposTracksToolbarHelpers
{
public:
    static void MakeAnimationSettingsEntries( FMenuBuilder& iMenuBuilder );
    static void MakeAnimationActorSettingsEntries( FMenuBuilder& iMenuBuilder );
    static void MakeCameraSettingsEntries( FMenuBuilder& iMenuBuilder );

    static void MakeCameraEntries( FMenuBuilder& iMenuBuilder, TSharedRef<FString> ioCameraName, FSimpleDelegate iOnTextCommit, bool iFocus = true );
    static void MakeAnimationEntries( FMenuBuilder& iMenuBuilder, TSharedRef<FString> ioAnimationName, FSimpleDelegate iOnTextCommit, bool iFocus = true );
};
