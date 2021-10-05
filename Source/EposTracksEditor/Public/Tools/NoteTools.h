// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class ISequencer;
class UMovieSceneSequence;
class UStoryNote;

/*
 * The note management
 */
class EPOSTRACKSEDITOR_API NoteTools
{
public:
    static UStoryNote* CreateNote( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName );
};
