// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationCutChannel.h"
#include "OdysseyAnimationTimelineSectionEditorData.generated.h"

class UOdysseyAnimationTimelineSection;
class FOdysseyRenderingChangedEvent;

/** Transient cut data and subscriptions owned by one animation timeline section. */
UCLASS()
class ODYSSEYANIMATIONTRACKSEDITOR_API UOdysseyAnimationTimelineSectionEditorData : public UObject
{
    GENERATED_BODY()

public:
    static UOdysseyAnimationTimelineSectionEditorData* GetOrCreate(UOdysseyAnimationTimelineSection& Section);

    virtual void BeginDestroy() override;
    virtual bool IsEditorOnly() const override { return true; }
    FOdysseyAnimationCutChannel& GetAnimationCutChannel();
    const FOdysseyAnimationCutChannel& GetAnimationCutChannel() const;
    FSimpleMulticastDelegate& OnAnimationCutChannelChanged();
    void UpdateAnimationCutChannel(const TArray<FKeyHandle>& KeyHandles, EPropertyChangeType::Type ChangeType);
    void RebuildAnimationCutChannel();

private:
    void Initialize();
    void OnSectionChanged();
    void OnAnimationChanged(const FOdysseyRenderingChangedEvent& Event);

    UPROPERTY(Transient)
    FOdysseyAnimationCutChannel AnimationCutChannel;

    FSimpleMulticastDelegate CutChannelChanged;
    bool bLockChannelRebuild = false;
};
