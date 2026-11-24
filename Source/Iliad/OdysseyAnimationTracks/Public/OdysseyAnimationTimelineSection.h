// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "MovieSceneSection.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"

#include "OdysseyAnimationTimelineSection.generated.h"

enum class EMovieSceneChannelProxyType : uint8;
class UOdysseyAnimation;
class UOdysseyAnimationPlayer;

/**
 * Defines the section for a template sequence track.
 */
UCLASS()
class ODYSSEYANIMATIONTRACKS_API UOdysseyAnimationTimelineSection
    : public UMovieSceneSection
{
    GENERATED_BODY()

public:
    static TRange<FFrameNumber> GetDefaultSectionRange(UOdysseyAnimationTimelineSection* iSection);

public:
    UOdysseyAnimationTimelineSection(const FObjectInitializer& ObjInitializer);

    virtual void SetRange( const TRange<FFrameNumber>& iNewRange );

public:
    UOdysseyAnimation* GetAnimation() const;
    EOdysseyAnimationPlayerPostBehaviour GetPreBehaviour() const;
    EOdysseyAnimationPlayerPostBehaviour GetPostBehaviour() const;
    FFrameNumber GetStartFrameOffset() const;

    void SetAnimation(UOdysseyAnimation* iAnimation);
    void SetPreBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);
    void SetPostBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);
    void SetStartFrameOffset(FFrameNumber iOffset);

    FFrameNumber ConvertFrameFromTimelineToSequence( FFrameNumber iFrameInTimeline );
    FFrameNumber ConvertFrameFromSequenceToTimeline( FFrameNumber iFrameInSequence );

#if WITH_EDITORONLY_DATA
    /**
     * The editor data of the section, a UOdysseyAnimationTimelineSectionEditorData holding the animation cut channel.
     * Created by the editor module (see UOdysseyAnimationTimelineSectionEditorData::GetOrCreate), never saved nor duplicated.
     */
    UObject* GetCutChannelEditorData() const;
    void SetCutChannelEditorData(UObject* Data);
#endif

protected:
    virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
    virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override;
    virtual void OnBindingIDsUpdated( const TMap<UE::MovieScene::FFixedObjectBindingID, UE::MovieScene::FFixedObjectBindingID>& OldFixedToNewFixedMap, FMovieSceneSequenceID LocalSequenceID, TSharedRef<UE::MovieScene::FSharedPlaybackState> SharedPlaybackState ) override;

protected:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    FFrameNumber StartFrameOffset = 0;

#if WITH_EDITORONLY_DATA
    UPROPERTY(Transient, DuplicateTransient)
    TObjectPtr<UObject> CutChannelEditorData;
#endif
};
