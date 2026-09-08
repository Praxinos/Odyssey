// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureDefines.h"
#include "Misc/Attribute.h"
#include "Misc/FrameTime.h"
#include "Tickable.h"

#if WITH_EDITOR
#include "TickableEditorObject.h"
#endif

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

UENUM(BlueprintType)
enum class EOdysseyAnimationPlayerPlayRange : uint8
{
    AnimationBounds,
    Infinite,
    Custom
};

#if WITH_EDITOR
    /*
    * Allows us to react to Tick events at runtime
    *
    * We expplicitely use FTickableEditorObject here because
    * using only FTickableEditorObject and setting IsTickableInEditor()
    * would prevent ticking while dragging sliders or while in a blocking window
    */
    using FOdysseyTickClass = FTickableEditorObject;
#else
    //Allows us to react to Tick events at runtime
    using FOdysseyTickClass = FTickableGameObject;
#endif

UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationPlayer
    : public UObject
    , public FOdysseyTickClass
{
    GENERATED_BODY()

public:
    UOdysseyAnimationPlayer();

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleDynamicMulticastDelegate);

    //C++ Events
    FSimpleMulticastDelegate& OnAnimationChanged();
    FSimpleMulticastDelegate& OnRenderTargetChanged();
    FSimpleMulticastDelegate& OnCursorFrameChanged();
    FSimpleMulticastDelegate& OnDisplayedFrameChanged();
    FSimpleMulticastDelegate& OnCurrentFrameChanged();
    FSimpleMulticastDelegate& OnStatusChanged();

protected:
    virtual void PostInitProperties() override;
    virtual void PostReinitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void Serialize(FArchive& Ar) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    virtual void PostEditImport() override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    void PropertyChanged(const FName& iPropertyName);
#endif

    void AnimationChanged();
    void LODGroupChanged();

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

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer", meta = (DisplayName = "Set IsLooping"))
    void SetIsLoopingInPlayRange(bool IsLooping);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer", meta = (DisplayName = "Get IsLooping"))
    bool GetIsLoopingInPlayRange() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetPlayRange(EOdysseyAnimationPlayerPlayRange PlayRange);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    EOdysseyAnimationPlayerPlayRange GetPlayRange() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetCustomPlayRange(FFrameNumber StartFrame, FFrameNumber EndFrame);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    void GetCustomPlayRange(FFrameNumber& StartFrame, FFrameNumber& EndFrame);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationPlayer")
    void SetRewindOnStop(bool Rewind);

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationPlayer")
    bool GetRewindOnStop() const;

    void SetLODGroup(enum TextureGroup iTextureGroup);
    enum TextureGroup GetLODGroup() const;

#if WITH_EDITOR
    void SetRenderType(TAttribute<uint64> iRenderType);
    uint64 GetRenderType() const;
    void SetIgnorePrePostBehaviour(bool iValue);
    bool GetIgnorePrePostBehaviour() const;
#endif

protected:
    // FTickableGameObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
    void InitializeRenderTarget();
    FFrameTime ApplyPreBehaviour(FFrameTime iFrame) const;
    FFrameTime ApplyPostBehaviour(FFrameTime iFrame) const;
    void UpdateTexture();
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

private:
    /**
     * The animation to play
     */
    UPROPERTY( EditAnywhere, Category="Animation" )
    TObjectPtr<UOdysseyAnimation> Animation;

    /*
    * Are we looping ?
    * If PlayRange is EOdysseyAnimationPlayerPlayRange::Custom, loop between the defined custom start and end frame
    * Otherwise, loop between the animation bounds (even if PlayRange is EOdysseyAnimationPlayerPlayRange::Infinite)
    */
    UPROPERTY( EditAnywhere, Category="Animation" )
    bool IsLoopingInPlayRange = true;

    /**
     * Defines the frame Range to play
     */
    UPROPERTY( EditAnywhere, Category="Animation" )
    EOdysseyAnimationPlayerPlayRange PlayRange = EOdysseyAnimationPlayerPlayRange::AnimationBounds; //Infinite, Custom

    /**
     * If PlayRange is EOdysseyAnimationPlayerPlayRange::Custom
     * defines the frame the Play should start at
     */
    UPROPERTY( EditAnywhere, Category="Animation", meta=(EditConditionHides, EditCondition="PlayRange==EOdysseyAnimationPlayerPlayRange::Custom") )
    FFrameNumber CustomPlayRangeStartFrame;

    /**
     * If PlayRange is EOdysseyAnimationPlayerPlayRange::Custom
     * defines the end frame the Play should end at
     */
    UPROPERTY( EditAnywhere, Category="Animation", meta=(EditConditionHides, EditCondition="PlayRange==EOdysseyAnimationPlayerPlayRange::Custom") )
    FFrameNumber CustomPlayRangeEndFrame;

    /**
     * If PlayRange is not AnimationBound
     * Defines the behaviour to use when playing before the start of the Animation's bounds
     */
    UPROPERTY( EditAnywhere, Category="Animation" )
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    /**
     * If PlayRange is not AnimationBound
     * Defines the behaviour to use when playing after the end of the Animation's bounds
     */
    UPROPERTY( EditAnywhere, Category="Animation" )
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    /**
     * The LODGroup to use for the RenderTarget in which the animation is Rendered
     */
    UPROPERTY( EditAnywhere, Category="Animation", meta=(DisplayName="Texture Group"), AssetRegistrySearchable )
    TEnumAsByte<enum TextureGroup> LODGroup = TEXTUREGROUP_Pixels2D;

    /**
     * If true, when Play stops or Stop() is called, automatically seeks to the frame on which the Play started
     */
    UPROPERTY( EditAnywhere, Category="Animation" )
    bool bRewindOnStop = false;

public:
    /**
     * Defines the speed of the animation in percentage of the animation framerate
     * 1.0 = 100% speed
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation" )
    float PlayRate = 1.0f; //1.0f means 100% of the animation framepersecond

    /**
     * The RenderTarget in which the animation is Rendered
     */
    UPROPERTY( Transient, DuplicateTransient )
    TObjectPtr<UTextureRenderTarget2D> RenderTarget;

public:
    //Blueprint Events

    /**
     * Fired when the player current frame changes (ex: when seeking or when playing)
     */
    UPROPERTY(BlueprintAssignable, Category = Events, meta = (DisplayName = "On Frame Changed"))
    FSimpleDynamicMulticastDelegate BP_OnFrameChanged;

    /**
     * Fired when the player status changes (ex: when paused or when stopped)
     */
    UPROPERTY(BlueprintAssignable, Category = Events, meta = (DisplayName = "On Status Changed"))
    FSimpleDynamicMulticastDelegate BP_OnStatusChanged;

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
    //C++ Events
    FSimpleMulticastDelegate mOnAnimationChanged;
    FSimpleMulticastDelegate mOnRenderTargetChanged;
    FSimpleMulticastDelegate mOnCurrentFrameChanged;
    FSimpleMulticastDelegate mOnDisplayedFrameChanged;
    FSimpleMulticastDelegate mOnCursorFrameChanged;
    FSimpleMulticastDelegate mOnStatusChanged;
};
