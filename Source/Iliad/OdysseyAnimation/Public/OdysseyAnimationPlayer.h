// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

//#include "OdysseyAnimation.h"
#include "CoreMinimal.h"
#include "TickableEditorObject.h"
#include "OdysseyInvalidTileMap.h"
#include "OdysseyRenderingAbility.h"

#include "OdysseyAnimationPlayer.generated.h"

class UOdysseyAnimation;

UENUM()
enum class EOdysseyAnimationPlayerStatus
{
    Playing,
    Paused,
    Stopped,
    Scrubbing
};



UENUM(BlueprintType)
enum class EOdysseyAnimationPlayerPostBehaviour : uint8
{
    Hold,
    Loop
};

UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationPlayer
    : public UObject
    , public FTickableGameObject //Allows us to react to Tick events
{
    GENERATED_BODY()

public:
    UOdysseyAnimationPlayer();

public:
    //Events
    FSimpleMulticastDelegate& OnAnimationChanged();
    FSimpleMulticastDelegate& OnCurrentFrameChanged();
    FSimpleMulticastDelegate& OnStatusChanged();

protected:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    void PropertyChanged(const FName& iPropertyName);
    void PostPropertyChanged(const FName& iPropertyName);

    void AnimationChanged();
    void StatusChanged();
    void PlayRateChanged();
    void IsLoopingChanged();

public:
    void Play(bool iBackward = false);
    void Pause();
    void Stop();

    void BeginScrub();
    void EndScrub();

    void SeekToFrame(FFrameTime iFrame);
    void SeekToFrameImmediate(FFrameTime iFrame);

    UTextureRenderTarget2D* GetRenderTarget();
    EOdysseyAnimationPlayerStatus GetStatus() const;

    FFrameTime GetCurrentFrame() const;

    void SetAnimation(UOdysseyAnimation* iAnimation);

    bool GetDuration(FFrameTime& oTime) const;

    bool GetFrameInAnimationBounds(FFrameTime iFrame, FFrameTime& oFrame) const;
    bool GetCurrentFrameInAnimationBounds(FFrameTime& oFrame) const;

    void SetPreBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);
    void SetPostBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);

    bool ApplyPreBehaviour(FFrameTime iFrame, FFrameTime& oFrame) const;
    bool ApplyPostBehaviour(FFrameTime iFrame, FFrameTime& oFrame) const;

    bool IsBackward() const;

    void SetRenderType(EOdysseyRenderingType iRenderType);
    EOdysseyRenderingType GetRenderType() const;

    void SetFrameRange(const TOptional<TRange<FFrameTime>>& iRange);

#if WITH_EDITOR
    void SetIgnoreAnimationBounds(bool iValue);
    bool GetIgnoreAnimationBounds() const;
#endif

protected:
    // FTickableGameObject implementation
    virtual bool IsTickableInEditor() const override { return true; }
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
    void UpdateTexture();
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

public:
    UPROPERTY()
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    float PlayRate = 1.0f; //1.0f means 100% of the animation framepersecond

    UPROPERTY()
    bool UsePreBehaviour = true;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY()
    bool UsePostBehaviour = true;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY()
    bool IsLooping = true;
public:
    UPROPERTY(Transient, DuplicateTransient)
    TObjectPtr<UTextureRenderTarget2D> RenderTarget;

private:
#if WITH_EDITOR
    bool mIgnoreAnimationBounds = false;
#endif
    bool mIsBackward = false;
    FFrameTime mCurrentFrame;
    TOptional<TRange<FFrameTime>> mRange;
    TArray<FGuid>   mImageRenderingComposition;
    FOdysseyInvalidTileMap mInvalidTileMap;
    EOdysseyAnimationPlayerStatus Status = EOdysseyAnimationPlayerStatus::Stopped;

    /**
     * We keep the renderer in memory to ensure all blocks are loaded and ready to be used instead of being recached
     */
    EOdysseyRenderingType mRenderType = EOdysseyRenderingType::Render;

private:
    //Events
    FSimpleMulticastDelegate mOnCurrentFrameChanged;
    FSimpleMulticastDelegate mOnStatusChanged;
};
