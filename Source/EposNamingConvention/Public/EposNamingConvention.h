// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class ACineCameraActor;
class APlaneActor;
class IMovieScenePlayer;
class UMovieSceneSequence;

class EPOSNAMINGCONVENTION_API NamingConvention
{
public:
    static FString GenerateCameraActorPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPathName, FString& oName );
    static FString GeneratePlaneActorPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPathName, FString& oName );

    static FString GenerateCameraTrackName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, ACineCameraActor* iCamera );
    static FString GeneratePlaneTrackName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, APlaneActor* iPlane );

public:
    static FString GenerateNoteAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName );

    static FString GenerateMaterialAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName );
    static FString GenerateTextureAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPath, FString& oName );
};
