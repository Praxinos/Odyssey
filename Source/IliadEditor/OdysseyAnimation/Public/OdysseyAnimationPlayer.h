// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

//#include "OdysseyAnimation.h"
#include "CoreMinimal.h"
#include "TickableEditorObject.h"
#include "ULISInvalidTileMap.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyImageRenderer.h"
#include "OdysseyImageRenderingAbility.h"

#include <ULIS>

#include "OdysseyAnimationPlayer.generated.h"

class UOdysseyAnimation;

UENUM()
enum class EOdysseyAnimationPlayerStatus
{
    Playing,
    Paused,
    Stopped
};



UENUM(BlueprintType)
enum class EOdysseyAnimationPlayerPostBehaviour : uint8
{
    None,
    Hold,
    Loop
};

UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationPlayer
    : public UObject
    , public FTickableEditorObject //Allows us to react to Tick events
{
    GENERATED_BODY()

public:
    UOdysseyAnimationPlayer();

public:
    //Events
    FSimpleMulticastDelegate& OnAnimationChanged();
    FSimpleMulticastDelegate& OnTextureChanged();
    FSimpleMulticastDelegate& OnCurrentTimeChanged();
    FSimpleMulticastDelegate& OnPlay();
    FSimpleMulticastDelegate& OnStop();

protected:
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    void PropertyChanged(const FName& iPropertyName);
    void PostPropertyChanged(const FName& iPropertyName);

    void AnimationChanged();
    void TextureChanged();
    void StatusChanged();
    void FrameRateChanged();
    void IsLoopingChanged();

public:
    void Play(bool iBackward = false);
    void Pause();
    void Stop();

    void SeekToTime(FTimespan iTime);
    void SeekToFrame(int iFrameIndex);
    void SeekToFrameImmediate(int iFrameIndex);

    UTexture2D* GetTexture();
    EOdysseyAnimationPlayerStatus GetStatus() const;

    FTimespan GetCurrentTime() const;

    bool GetDuration(FTimespan& oTime) const;

    bool GetCurrentTimeInPlayerBounds(FTimespan& oTime) const;
    bool GetCurrentFrameInAnimationBounds(int& oFrame) const;

    void SetPreBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);
    void SetPostBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);

    bool ApplyPreBehaviour(FTimespan iTime, FTimespan& oTime) const;
    bool ApplyPostBehaviour(FTimespan iTime, FTimespan& oTime) const;

    bool IsBackward() const;

    void SetRenderType(IOdysseyImageRenderer::eRenderType iRenderType);
    IOdysseyImageRenderer::eRenderType GetRenderType() const;

    void SetFrameRange(const TOptional<FInt32Range>& iRange);

#if WITH_EDITOR
    void SetIgnoreAnimationBounds(bool iValue);
    bool GetIgnoreAnimationBounds() const;
#endif

protected:
    // FTickableEditorObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
    void UpdateTexture();
    void CopyBlocksToTexture(const TArray<TSharedPtr<::ULIS::FBlock>>& iBlocks, const TArray<::ULIS::FRectI>& iRects);
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);

public:
    UPROPERTY()
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    double FrameRate = 1.0f; //1.0f means 100% of the animation framepersecond

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
    TObjectPtr<UTexture2D> Texture;

private:
#if WITH_EDITOR
    bool mIgnoreAnimationBounds = false;
#endif
    bool mIsBackward = false;
    FTimespan mCurrentTime;
    TOptional<TRange<FTimespan>> mRange;
    TArray<FGuid>   mImageRenderingComposition;
    FULISInvalidTileMap mInvalidTileMap;
    EOdysseyAnimationPlayerStatus Status = EOdysseyAnimationPlayerStatus::Stopped;

    /**
     * We keep the renderer in memory to ensure all blocks are loaded and ready to be used instead of being recached
     */
    TSharedPtr<IOdysseyImageRenderer> mRenderer;
    IOdysseyImageRenderer::eRenderType mRenderType = IOdysseyImageRenderer::eRenderType::Render;

private:
    //Events
    FSimpleMulticastDelegate mOnAnimationChanged;
    FSimpleMulticastDelegate mOnTextureChanged;
    FSimpleMulticastDelegate mOnCurrentTimeChanged;
    FSimpleMulticastDelegate mOnPlay;
    FSimpleMulticastDelegate mOnStop;
};
