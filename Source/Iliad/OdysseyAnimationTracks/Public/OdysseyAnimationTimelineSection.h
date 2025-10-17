// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "MovieSceneSection.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"

#include "OdysseyAnimationCutChannel.h"

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

    FOdysseyAnimationCutChannel& GetAnimationCutChannel();
    const FOdysseyAnimationCutChannel& GetAnimationCutChannel() const;

    DECLARE_MULTICAST_DELEGATE( FOnAnimationCutChannelChanged );
    FOnAnimationCutChannelChanged& OnAnimationCutChannelChanged();

    void UpdateAnimationCutChannel( const TArray<FKeyHandle>& iKeyHandles, EPropertyChangeType::Type iChangeType );

protected:
    virtual void PostInitProperties() override;
    virtual void PostEditImport() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate( EDuplicateMode::Type DuplicateMode ) override;
#if WITH_EDITOR
    virtual void PreEditChange( FProperty* PropertyAboutToChange );
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent );
#endif

    virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
    virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override;
    virtual void OnBindingIDsUpdated( const TMap<UE::MovieScene::FFixedObjectBindingID, UE::MovieScene::FFixedObjectBindingID>& OldFixedToNewFixedMap, FMovieSceneSequenceID LocalSequenceID, TSharedRef<UE::MovieScene::FSharedPlaybackState> SharedPlaybackState ) override;

    virtual void RebuildAnimationCutChannel();

    virtual void OnAnimationChanged( const FOdysseyRenderingChangedEvent& iEvent );

protected:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    FFrameNumber StartFrameOffset = 0;

    UPROPERTY()
    FOdysseyAnimationCutChannel AnimationCutChannel;

    FOnAnimationCutChannelChanged mOnAnimationCutChannelChanged;

    bool LockChannelRebuild = false;
};
