// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationCurrentFrameMutator.h"

#include "OdysseyAnimationPlayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Misc/OdysseyUndoDelegates.h"

FOdysseySetCurrentFrameMutation::FOdysseySetCurrentFrameMutation(UOdysseyAnimationPlayer* iPlayer, int iFrame)
    : mPlayer(iPlayer)
    , mFrame(iFrame)
{
}

void
FOdysseySetCurrentFrameMutation::Apply()
{
    mPlayer->SeekToFrame(mFrame);
}

void
FOdysseySetCurrentFrameMutation::Revert()
{
    mPlayer->SeekToFrame(mFrame);
}

FOdysseyAnimationCurrentFrameMutator::FOdysseyAnimationCurrentFrameMutator(UOdysseyAnimationPlayer* iPlayer)
    : FOdysseyMutator(iPlayer, "FOdysseyAnimationCurrentFrameMutator")
    , mPlayer(iPlayer)
{
}

void
FOdysseyAnimationCurrentFrameMutator::Set(int iFrame)
{
    TSharedRef<FOdysseySetCurrentFrameMutation> mutation = MakeShared<FOdysseySetCurrentFrameMutation>(mPlayer, iFrame);
    AddMutation(mutation);
    if (iFrame != mPlayer->GetCurrentFrame())
        ApplyMutation(mutation);
}
