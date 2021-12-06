// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposNamingConvention.h"

#include "CineCameraActor.h"

#include "IMovieScenePlayer.h"
#include "MovieSceneSequence.h"
#include "PlaneActor.h"

#define LOCTEXT_NAMESPACE "NamingConvention"

//---

//static
FString
NamingConvention::GenerateCameraActorPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    oPath = *FPaths::GetBaseFilename( iRootSequence->GetPathName() );
    oName = TEXT( "Camera_1" );

    return FPaths::Combine( oPath, oName );
}

//static
FString
NamingConvention::GeneratePlaneActorPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString camera_path;
    FString camera_name;
    GenerateCameraActorPathName( iSequencer, iRootSequence, iSequence, camera_path, camera_name );

    oPath = camera_path;
    oName = TEXT( "Plane_1" );

    return FPaths::Combine( oPath, oName );
}

//static
FString
NamingConvention::GenerateCameraTrackName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, ACineCameraActor* iCamera )
{
    return iCamera->GetActorLabel();
}

//static
FString
NamingConvention::GeneratePlaneTrackName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, APlaneActor* iPlane )
{
    return iPlane->GetActorLabel();
}

#undef LOCTEXT_NAMESPACE
