// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

//#include "OdysseyAnimation.h"
#include "CoreMinimal.h"
#include "Tickable.h"
#include "OdysseyInvalidTileMap.h"
#include "OdysseyRenderingAbility.h"
#include "Misc/Attribute.h"
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

UENUM(BlueprintType)
enum class EOdysseyAnimationPlayerPlayRange : uint8
{
    AnimationBounds,
    Infinite,
    Custom
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
    FSimpleMulticastDelegate& OnCursorFrameChanged();
    FSimpleMulticastDelegate& OnCurrentFrameChanged();
    FSimpleMulticastDelegate& OnDisplayedFrameChanged();

protected:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void Serialize(FArchive& Ar) override;

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

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    UTextureRenderTarget2D* GetRenderTarget();

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    EOdysseyAnimationPlayerStatus GetStatus() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    FFrameTime GetCursorFrame() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    FFrameTime GetCurrentFrame() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    FFrameTime GetDisplayedFrame() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetAnimation(UOdysseyAnimation* iAnimation);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    FFrameTime ApplyPrePostBehaviour(FFrameTime iFrame) const;

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

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetIsLoopingInPlayRange(bool IsLooping);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    bool GetIsLoopingInPlayRange() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetPlayRange(EOdysseyAnimationPlayerPlayRange PlayRange);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    EOdysseyAnimationPlayerPlayRange GetPlayRange() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetCustomPlayRange(FFrameNumber StartFrame, FFrameNumber EndFrame);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    void GetCustomPlayRange(FFrameNumber& StartFrame, FFrameNumber& EndFrame);

#if WITH_EDITOR
    void SetRenderType(TAttribute<uint64> iRenderType);
    uint64 GetRenderType() const;
    void SetIgnorePrePostBehaviour(bool iValue);
    bool GetIgnorePrePostBehaviour() const;
#endif

protected:
    // FTickableGameObject implementation
    virtual bool IsTickableInEditor() const override { return true; }
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
    FFrameTime ApplyPreBehaviour(FFrameTime iFrame) const;
    FFrameTime ApplyPostBehaviour(FFrameTime iFrame) const;
    void UpdateTexture();
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

private:
    UPROPERTY()
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    bool IsLoopingInPlayRange = false;

    UPROPERTY()
    EOdysseyAnimationPlayerPlayRange PlayRange = EOdysseyAnimationPlayerPlayRange::AnimationBounds; //Infinite, Custom

    UPROPERTY()
    FFrameNumber CustomPlayRangeStartFrame;

    UPROPERTY()
    FFrameNumber CustomPlayRangeEndFrame;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

public:
    UPROPERTY()
    float PlayRate = 1.0f; //1.0f means 100% of the animation framepersecond

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> RenderTarget;

    int mPatchDelayFirstRender = 0;

private:
#if WITH_EDITOR
    //Allows to have Infinite Animation Scrubbing in Editor
    bool mIgnorePrePostBehaviour = false;
#endif

    TAttribute<uint64> mRenderType = EOdysseyRenderingType::Render;
    bool mIsBackward = false;
    FFrameTime mCursorFrame; //The frame on which the player cursor is (does not correspond to timeline cursor when cursorframe is in Pre/PostBehaviour range)
    FFrameTime mCurrentFrame; //Current frame when stopped (also the frame currently edited by the editor)
    FFrameTime mDisplayedFrame; //Displayed frame when playing / Scrubbing / Paused (always in animation bounds)
    TArray<FGuid>   mImageRenderingComposition;
    FOdysseyInvalidTileMap mInvalidTileMap;
    EOdysseyAnimationPlayerStatus Status = EOdysseyAnimationPlayerStatus::Stopped;

private:
    //Events
    FSimpleMulticastDelegate mOnCurrentFrameChanged;
    FSimpleMulticastDelegate mOnDisplayedFrameChanged;
    FSimpleMulticastDelegate mOnCursorFrameChanged;
};
