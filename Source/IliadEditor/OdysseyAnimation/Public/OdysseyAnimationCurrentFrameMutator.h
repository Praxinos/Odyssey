// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once
#include "Mutations/OdysseyMutator.h"

class UOdysseyAnimation;

class ODYSSEYANIMATION_API FOdysseyAnimationCurrentFrameMutator
    : public FOdysseyMutator
{
public:
    virtual ~FOdysseyAnimationCurrentFrameMutator() {};
    FOdysseyAnimationCurrentFrameMutator(UOdysseyAnimation* iAnimation);

public:
    void Set(int iFrame);

private:
    UOdysseyAnimation* mAnimation;
};

class  FOdysseySetCurrentFrameMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetCurrentFrameMutation(UOdysseyAnimation* iAnimation, int iFrame);

public:
    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    UOdysseyAnimation* mAnimation;
    int mFrame;
};
