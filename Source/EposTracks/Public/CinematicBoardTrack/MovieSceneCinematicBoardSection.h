// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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

    /** Object constructor. */
    UMovieSceneCinematicBoardSection( const FObjectInitializer& ObjInitializer );

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    virtual void PreEditChange( FProperty* PropertyAboutToChange ) override;
#endif

public:
    /** @return The board display name. if empty, returns the sequence's name*/
    UFUNCTION( BlueprintPure, Category = "Sequencer|Section" )
    FString GetBoardDisplayName() const;

    /** Set the board display name */
    UFUNCTION( BlueprintCallable, Category = "Sequencer|Section" )
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

public:
    /** Get the height of this track's rows */
    float GetWidgetHeight() const;

    /** Set the height of this track's rows */
    void SetWidgetHeight( TAttribute<float> iWidgetHeight );

private:
    /** The height for each row of this track */
    TAttribute<float> mWidgetHeight;
#endif

//---

#if WITH_EDITOR

private:
    TRange<FFrameNumber> mSectionRangeBackup;

//---

private:
    bool IsSequenceResizable( const UMovieSceneSection* iSection ) const;

    bool IsResizableLeadingEdge(); // should be const ... doesn't work with TArray::Find() ...
    bool IsResizableTrailingEdge(); // should be const ...

public:
    void ResizeLeadingEdge( FFrameNumber iNewFrame );
    void ResizeTrailingEdge( FFrameNumber iNewFrame );

public:
    void StartResizing();
    void Resizing();
    void StopResizing();

    bool IsResizing() const;
    bool IsResizingLeading() const;
    bool IsResizingTrailing() const;

private:
    int mResizing { -1 };
    bool mLeadingResizing { false };
    bool mTrailingResizing { false };

//---

public:
    bool GuessStartMoving( TRange<FFrameNumber>& iRangeBackup ); //PATCH

    void StartMoving();
    void Moving();
    void StopMoving();

    bool IsMoving() const;

private:
    int mMoving { -1 };

#endif
};
