// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

//---

// Store data about a subsequence using the hierarchy/ID to get camera/planes/...
struct FInnerSequenceData
{
public:
    FInnerSequenceData();
    FInnerSequenceData( const FMovieSceneSequenceIDRef iID, const FMovieSceneSequenceHierarchy* iHierarchy, IMovieScenePlayer* ioPlayer );

    void InitializeFromSubSection( IMovieScenePlayer* iPlayer, FMovieSceneSequenceIDRef iSequenceID, const UMovieSceneSubSection& iSubSection );
    void Fill();

    bool IsInitialized() const;
    bool IsFilled() const;

    FMovieSceneSequenceID   GetInnerSequenceID() const;
    UMovieSceneSequence*    GetInnerSequence() const;
    UMovieScene*            GetInnerMovieScene() const;

public:
    FMovieSceneSequenceID                   mInnerSequenceID;
    const FMovieSceneSequenceHierarchy*     mHierarchy;
    IMovieScenePlayer*                      mPlayer;

    const FMovieSceneSequenceHierarchyNode* mNode;
    const FMovieSceneSubSequenceData*       mSubData;
    UMovieSceneSequence*                    mInnerMovieSceneSequence;
    UMovieScene*                            mInnerMovieScene;
};

//---

class UMovieScene3DTransformSection;
class UMovieScenePrimitiveMaterialSection;

class CinematicBoardSectionBindingHelpers
{
public:
    /** Get all (static mesh) possessables inside the given section
        This is used by GetMaxPlaneBindings() which loops over all sections in the track
        to get section with the max number of planes
    */
    static TArray<FMovieScenePossessable> GetPlaneBindings( const UMovieSceneSubSection& iSection, ISequencer& iSequencer );

    /** Get the maximum number of planes inside all subsections of the current track */
    static int GetMaxPlaneBindings( const UMovieSceneTrack& iTrack, ISequencer& iSequencer );

public:
    /** Get the camera possessable inside the current subsection */
    static FMovieScenePossessable GetCameraBinding( const UMovieSceneSubSection& iSection, ISequencer& iSequencer );

    static TArray<UMovieScene3DTransformSection*> GetCameraTransformSections( UMovieSceneSequence* iInnerSequence, ISequencer& iSequencer );
    static TArray<UMovieScene3DTransformSection*> GetPlaneTransformSections( UMovieSceneSequence* iInnerSequence, const FMovieScenePossessable& iPossessable, ISequencer& iSequencer );
    static TArray<UMovieScenePrimitiveMaterialSection*> GetPlaneMaterialSections( UMovieSceneSequence* iInnerSequence, const FMovieScenePossessable& iPossessable, ISequencer& iSequencer );
};

//---

struct FMetaFloatChannel;
struct FMetaMaterialChannel;

class CinematicBoardSectionKeysHelpers
{
public:
    static TArray<double> BuildThumbnailKeys( const UMovieSceneSubSection& iSubSection );

    static TSharedPtr<FMovieSceneChannelProxy> BuildCameraTransformChannelProxy( const UMovieSceneSubSection& iSubSection, ISequencer& iSequencer );
    static TSharedPtr<FMetaFloatChannel> BuildCameraTransformMetaChannel( const TSharedPtr<FMovieSceneChannelProxy> iChannelProxy, const FFrameNumber& iMergeTolerance );

    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesTransformChannelProxy( const UMovieSceneSubSection& iSubSection, ISequencer& iSequencer );
    static TMap<FGuid, TSharedPtr<FMetaFloatChannel>> BuildPlanesTransformMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance );

    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesMaterialChannelProxy( const UMovieSceneSubSection& iSubSection, ISequencer& iSequencer );
    static TMap<FGuid, TSharedPtr<FMetaMaterialChannel>> BuildPlanesMaterialMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance );

private:
    static TArray<FFrameTime> FindCameraTransformKeysRecursive( const UMovieSceneSubSection& iSubSection );
};
