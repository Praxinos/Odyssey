// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

//#include "OdysseyAnimation.h"
#include "CoreMinimal.h"
#include "Tickable.h"
#include "OdysseyInvalidTileMap.h"
#include "OdysseyRenderingAbility.h"
#include "Misc/FrameTime.h"

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
    FSimpleMulticastDelegate& OnCurrentFrameChanged();
    FSimpleMulticastDelegate& OnDisplayedFrameChanged();

protected:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    void PropertyChanged(const FName& iPropertyName);
#endif

    void AnimationChanged();

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|AnimationPlayer")
    void Play(bool iBackward = false);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void Pause();

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void Stop();

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SeekToFrame(FFrameTime iFrame);
    void SeekToFrameImmediate(FFrameTime iFrame);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    UTextureRenderTarget2D* GetRenderTarget();

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    EOdysseyAnimationPlayerStatus GetStatus() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    FFrameTime GetCurrentFrame() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    FFrameTime GetDisplayedFrame() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetAnimation(UOdysseyAnimation* iAnimation);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    bool GetDuration(FFrameTime& oTime) const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    bool GetFrameInAnimationBounds(FFrameTime iFrame, FFrameTime& oFrame) const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    bool GetCurrentFrameInAnimationBounds(FFrameTime& oFrame) const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    bool GetDisplayedFrameInAnimationBounds(FFrameTime& oFrame) const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetPreBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetPostBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    bool IsBackward() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void BeginScrub();

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void EndScrub();

#if WITH_EDITOR
    void SetUsePreBehaviour(bool iValue);
    void SetUsePostBehaviour(bool iValue);
    bool UsePreBehaviour() const;
    bool UsePostBehaviour() const;
    bool IsLooping() const;
    void SetIsLooping(bool iIsLooping);
    void SetRenderType(uint64 iRenderType);
    uint64 GetRenderType() const;
    void SetFrameRange(const TOptional<TRange<FFrameTime>>& iRange);
    void SetIgnoreAnimationBounds(bool iValue);
    bool GetIgnoreAnimationBounds() const;
#endif

protected:
    // FTickableGameObject implementation
    virtual bool IsTickableInEditor() const override { return true; }
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
    bool ApplyPreBehaviour(FFrameTime iFrame, FFrameTime& oFrame) const;
    bool ApplyPostBehaviour(FFrameTime iFrame, FFrameTime& oFrame) const;
    void UpdateTexture();
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

private:
    UPROPERTY()
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

public:
    UPROPERTY()
    float PlayRate = 1.0f; //1.0f means 100% of the animation framepersecond

    UPROPERTY(Transient, DuplicateTransient)
    TObjectPtr<UTextureRenderTarget2D> RenderTarget;

private:
#if WITH_EDITOR
    bool mUsePreBehaviour = true;
    bool mUsePostBehaviour = true;
    bool mIgnoreAnimationBounds = false;
    TOptional<TRange<FFrameTime>> mRange;
    bool mIsLooping = true;
#endif

    uint64 mRenderType = EOdysseyRenderingType::Render;
    uint64 mScrubInitialRenderType = EOdysseyRenderingType::Render;
    bool mIsBackward = false;
    FFrameTime mCurrentFrame; //Current frame when stopped
    FFrameTime mDisplayedFrame; //Displayed frame when playing / Scrubbing / Paused
    TArray<FGuid>   mImageRenderingComposition;
    FOdysseyInvalidTileMap mInvalidTileMap;
    EOdysseyAnimationPlayerStatus Status = EOdysseyAnimationPlayerStatus::Stopped;

private:
    //Events
    FSimpleMulticastDelegate mOnCurrentFrameChanged;
    FSimpleMulticastDelegate mOnDisplayedFrameChanged;
};
