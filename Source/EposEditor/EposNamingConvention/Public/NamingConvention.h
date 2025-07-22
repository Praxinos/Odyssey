// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneSequenceID.h"

#include "Settings/NamingConventionSettings.h"

class ACineCameraActor;
class AOdysseyAnimationActor;
class IMovieScenePlayer;
class UEposMovieSceneSequence;
class UMovieSceneSequence;
class UMovieSceneSubSection;
struct FBoardNameElements;
struct FShotNameElements;

class EPOSNAMINGCONVENTION_API NamingConvention
{
public:
    static FString GenerateCameraActorPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPathName, FString& oName );
    static FString GenerateAnimationActorPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPathName, FString& oName );

    static FString GenerateCameraTrackName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, ACineCameraActor* iCamera );
    static FString GenerateAnimationTrackName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, AOdysseyAnimationActor* iAnimation );

public:
    static FString GenerateNoteAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPath, FString& oName );

    static FString GenerateAnimationAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPath, FString& oName );

public:
    static FString GenerateBoardAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iParentSequence, FMovieSceneSequenceIDRef iParentSequenceID, FFrameNumber iFrameNumber, FString& oPath, FString& oName, FBoardNameElements& oElements );

    static FString GenerateShotAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iParentSequence, FMovieSceneSequenceIDRef iParentSequenceID, FFrameNumber iFrameNumber, FString& oPath, FString& oName, FShotNameElements& oElements );
    static FString GenerateTakeAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iParentSequence, FMovieSceneSequenceIDRef iParentSequenceID, UMovieSceneSubSection* iSubSection, FString& oPath, FString& oName, FShotNameElements& oElements );

private:
    static FString GetRootPath( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence );
};
