// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Sections/MovieSceneSubSection.h"
#include "MovieSceneCinematicBoardSection.generated.h"

/**
 * Implements a board section.
 */
UCLASS( BlueprintType )
class EPOSTRACKS_API UMovieSceneCinematicBoardSection
    : public UMovieSceneSubSection
{
    GENERATED_BODY()

    /** Default constructor. */
    UMovieSceneCinematicBoardSection();

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    virtual void PreEditChange( FProperty* PropertyAboutToChange ) override;
#endif

public:
    /** @return The board display name */
    UFUNCTION( BlueprintPure, Category = "Movie Scene Section" )
    FString GetBoardDisplayName() const;

    /** Set the board display name */
    UFUNCTION( BlueprintCallable, Category = "Movie Scene Section" )
    void SetBoardDisplayName( const FString& BoardDisplayName );

private:
    /** The Board's display name */
    UPROPERTY()
    FString mBoardDisplayName;

#if WITH_EDITORONLY_DATA
public:
    /** @return The board thumbnail reference frame offset from the start of this section */
    float GetThumbnailReferenceOffset() const;

    /** Set the thumbnail reference offset */
    void SetThumbnailReferenceOffset( float iNewOffset );

private:

    /** The board's reference frame offset for single thumbnail rendering */
    UPROPERTY()
    float mThumbnailReferenceOffset;
#endif

public:
    void StartResizing();
    void Resizing();
    void StopResizing();
    bool IsResizing() const;

    FMovieSceneFrameRange GetTrueRangeBackup() const;

private:
    int mResizing;

    FMovieSceneFrameRange mSectionRangeBackup;
};
