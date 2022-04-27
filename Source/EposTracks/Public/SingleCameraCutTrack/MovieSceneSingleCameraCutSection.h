// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Curves/KeyHandle.h"
#include "EntitySystem/IMovieSceneEntityProvider.h"
#include "EntitySystem/MovieSceneEntityIDs.h"
#include "Misc/Guid.h"
#include "MovieSceneObjectBindingID.h"
#include "MovieSceneSection.h"
#include "UObject/ObjectMacros.h"
#include "UObject/SequencerObjectVersion.h"
#include "MovieSceneSingleCameraCutSection.generated.h"

struct FMovieSceneSequenceID;
class IMovieScenePlayer;
class UCameraComponent;

/**
 * Movie CameraCuts are sections on the CameraCuts track, that show what the viewer "sees"
 */
UCLASS(MinimalAPI)
class UMovieSceneSingleCameraCutSection
    : public UMovieSceneSection
    , public IMovieSceneEntityProvider
{
    GENERATED_BODY()

public:
    /** Constructs a new camera cut section */
    UMovieSceneSingleCameraCutSection(const FObjectInitializer& Init);

    /** Sets the camera binding for this CameraCut section. Evaluates from the sequence binding ID */
    void SetCameraGuid( const FGuid& InGuid )
    {
        SetCameraBindingID( UE::MovieScene::FRelativeObjectBindingID( InGuid ) );
    }

    /** Gets the camera binding for this CameraCut section */
    UFUNCTION(BlueprintPure, Category = "Sequencer|Section")
    const FMovieSceneObjectBindingID& GetCameraBindingID() const
    {
        return CameraBindingID;
    }

    /** Sets the camera binding for this CameraCut section */
    UFUNCTION(BlueprintCallable, Category = "Sequencer|Section")
    void SetCameraBindingID(const FMovieSceneObjectBindingID& InCameraBindingID)
    {
        CameraBindingID = InCameraBindingID;
    }

    //---

    //~ UMovieSceneSection interface
    virtual void OnBindingIDsUpdated( const TMap<UE::MovieScene::FFixedObjectBindingID, UE::MovieScene::FFixedObjectBindingID>& OldFixedToNewFixedMap, FMovieSceneSequenceID LocalSequenceID, const FMovieSceneSequenceHierarchy* Hierarchy, IMovieScenePlayer& Player ) override;
    virtual void GetReferencedBindings(TArray<FGuid>& OutBindings) override;
    /**
     * Sets a new range of times for this section
     *
     * @param NewRange  The new range of times
     */
    EPOSTRACKS_API virtual void SetRange( const TRange<FFrameNumber>& NewRange );
    /**
     * Set this section's end frame in sequence resolution space
     * @note: Will be clamped to the current start frame if necessary
     */
    EPOSTRACKS_API virtual void SetStartFrame( TRangeBound<FFrameNumber> NewEndFrame );
    virtual void SetStartFrameAuto();
    /**
     * Split a section in two at the split time
     *
     * @param SplitTime The time at which to split
     * @param bDeleteKeys Delete keys outside the split ranges
     * @return The newly created split section
     */
    EPOSTRACKS_API virtual UMovieSceneSection* SplitSection( FQualifiedFrameTime SplitTime, bool bDeleteKeys );
    /**
     * Trim a section at the trim time
     *
     * @param TrimTime The time at which to trim
     * @param bTrimLeft Whether to trim left or right
     * @param bDeleteKeys Delete keys outside the split ranges
     */
    EPOSTRACKS_API virtual void TrimSection( FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys );

    //---

    /** ~UObject interface */
    virtual void PostLoad() override;

    //---

    /**
     * Resolve a camera component for this cut section from the specified player and sequence ID
     *
     * @param Player     The sequence player to use to resolve the object binding for this camera
     * @param SequenceID The sequence ID for the specific instance that this section exists within
     *
     * @return A camera component to be used for this cut section, or nullptr if one was not found.
     */
    EPOSTRACKS_API UCameraComponent* GetFirstCamera(IMovieScenePlayer& Player, FMovieSceneSequenceID SequenceID) const;

#if WITH_EDITOR
    EPOSTRACKS_API virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    /**
     * Computes the transform of the bound camera at the section's start time.
     * This is for internal use by UMovieSceneCameraCutTrack during pre-compilation.
     */
    void ComputeInitialCameraCutTransform();

private:
    virtual void ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity) override;

private:
    /** The camera binding that this movie CameraCut uses */
    UPROPERTY(EditAnywhere, Category="Section")
    FMovieSceneObjectBindingID CameraBindingID;

    /** Camera transform at the start of the cut, computed at compile time */
    UPROPERTY()
    FTransform InitialCameraCutTransform;
    UPROPERTY()
    bool bHasInitialCameraCutTransform = false;

#if WITH_EDITORONLY_DATA
public:
    /** @return The thumbnail reference frame offset from the start of this section */
    float GetThumbnailReferenceOffset() const
    {
        return ThumbnailReferenceOffset;
    }

    /** Set the thumbnail reference offset */
    void SetThumbnailReferenceOffset(float InNewOffset)
    {
        Modify();
        ThumbnailReferenceOffset = InNewOffset;
    }

private:

    /** The reference frame offset for single thumbnail rendering */
    UPROPERTY()
    float ThumbnailReferenceOffset;
#endif

    friend class UMovieSceneSingleCameraCutTrackInstance;
};
