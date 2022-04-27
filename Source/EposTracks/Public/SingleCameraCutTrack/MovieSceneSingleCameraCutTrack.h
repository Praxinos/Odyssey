// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Guid.h"
#include "MovieSceneNameableTrack.h"
#include "MovieSceneObjectBindingID.h"
#include "MovieSceneSingleCameraCutTrack.generated.h"

class UMovieSceneSingleCameraCutSection;

/**
 * Handles manipulation of CameraCut properties in a movie scene.
 */
UCLASS(MinimalAPI)
class UMovieSceneSingleCameraCutTrack
    : public UMovieSceneNameableTrack
{
    GENERATED_BODY()
    UMovieSceneSingleCameraCutTrack( const FObjectInitializer& ObjectInitializer );

public:

    /**
     * Adds a new CameraCut at the specified time.
     *
     * @param CameraBindingID Handle to the camera that the CameraCut switches to when active.
     * @param Time The within this track's movie scene where the CameraCut is initially placed.
     * @return The newly created camera cut section
     */
    EPOSTRACKS_API UMovieSceneSingleCameraCutSection* AddNewSingleCameraCut(const FMovieSceneObjectBindingID& CameraBindingID, FFrameNumber Time);

public:

    // UMovieSceneTrack interface
    virtual void AddSection(UMovieSceneSection& Section) override;
    virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
    virtual UMovieSceneSection* CreateNewSection() override;
    virtual bool SupportsMultipleRows() const override;
    virtual EMovieSceneTrackEasingSupportFlags SupportsEasing(FMovieSceneSupportsEasingParams& Params) const override;
    virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
    virtual bool HasSection( const UMovieSceneSection& Section ) const override;
    virtual bool IsEmpty() const override;
    virtual void RemoveSection(UMovieSceneSection& Section) override;
    virtual void RemoveSectionAt(int32 SectionIndex) override;
    virtual void RemoveAllAnimationData() override;

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
#endif

#if WITH_EDITOR
    virtual EMovieSceneSectionMovedResult OnSectionMoved(UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params) override;
#endif

    EPOSTRACKS_API FFrameNumber FindEndTimeForCameraCut(FFrameNumber StartTime);

protected:

    virtual void PreCompileImpl( FMovieSceneTrackPreCompileResult& OutPreCompileResult ) override;

private:

    /** All movie scene sections. */
    UPROPERTY()
    TArray<TObjectPtr<UMovieSceneSection>> Sections;
};
