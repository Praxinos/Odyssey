// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCurrentFrameMutator.h"

#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"

FOdysseySetCurrentFrameMutation::FOdysseySetCurrentFrameMutation(UOdysseyAnimation* iAnimation, int iFrame)
	: mAnimation(iAnimation)
	, mFrame(iFrame)
{
}

void
FOdysseySetCurrentFrameMutation::Apply()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, "CurrentFrame", mFrame);
}

void
FOdysseySetCurrentFrameMutation::Revert()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, "CurrentFrame", mFrame);
}

FOdysseyAnimationCurrentFrameMutator::FOdysseyAnimationCurrentFrameMutator(UOdysseyAnimation* iAnimation)
	: FOdysseyMutator(iAnimation, "FOdysseyAnimationCurrentFrameMutator")
	, mAnimation(iAnimation)
{
}

void
FOdysseyAnimationCurrentFrameMutator::Set(int iFrame)
{
	TSharedRef<FOdysseySetCurrentFrameMutation> mutation = MakeShared<FOdysseySetCurrentFrameMutation>(mAnimation, iFrame);
	AddMutation(mutation);
	if (iFrame != mAnimation->CurrentFrame)
		ApplyMutation(mutation);
}
