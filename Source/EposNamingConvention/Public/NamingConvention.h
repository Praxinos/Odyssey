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
    static FString GenerateCameraActorPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPathName, FString& oName );
    static FString GeneratePlaneActorPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPathName, FString& oName );

    static FString GenerateCameraTrackName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, ACineCameraActor* iCamera );
    static FString GeneratePlaneTrackName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, APlaneActor* iPlane );

public:
    static FString GetMasterMaterialPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName );
    static FString GetMasterTexturePathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName );

public:
    static FString GenerateNoteAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName );

    static FString GenerateMaterialAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName );
    static FString GenerateTextureAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPath, FString& oName );

public:
    struct FBoardComponents
    {
        int32 mNextIndex;

        FString mStudioName;
        FString mStudioAccronym;
        FString mProductionName;
        FString mProductionAccronym;
        FString mInitials;
    };
    static FString GenerateBoardAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName, FBoardComponents& oComponents );

    struct FShotComponents
        : public FBoardComponents
    {
        int32 mNextTake;
    };
    static FString GenerateShotAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName, FShotComponents& oComponents );

    static FString GenerateSequenceAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, UClass* iType, FString& oPath, FString& oName );

private:
    static FString GetRootPath( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence );
    static FString GetMasterPath( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence );
};
