// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"

#include "OdysseyEditorShortcuts.h"

class FUICommandList;
class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationGlobalShortcuts
    : public FOdysseyEditorShortcuts
{
public:
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int> /*iFrame*/)

public:
    virtual ~FOdysseyAnimationGlobalShortcuts() {};
    FOdysseyAnimationGlobalShortcuts(
        const TAttribute<UOdysseyAnimation*>& iAnimation,
        const TAttribute<int>& iCurrentFrame,
        const FOnTransactCurrentFrame& iOnTransactCurrentFrame
    );

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TAttribute<UOdysseyAnimation*> mAnimation;
};
