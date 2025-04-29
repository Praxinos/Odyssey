// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once
#include "Mutations/OdysseyMutator.h"

class UOdysseyAnimationPlayer;

class FOdysseyAnimationCurrentFrameMutator
    : public FOdysseyMutator
{
public:
    virtual ~FOdysseyAnimationCurrentFrameMutator() {};
    FOdysseyAnimationCurrentFrameMutator(UOdysseyAnimationPlayer* iPlayer);

public:
    void Set(int iFrame);

private:
    UOdysseyAnimationPlayer* mPlayer;
};

class  FOdysseySetCurrentFrameMutation
    : public IOdysseyMutation
{
public:
    FOdysseySetCurrentFrameMutation(UOdysseyAnimationPlayer* iPlayer, int iFrame);

public:
    //Applies the mutation
    virtual void Apply() override;

    //Reverts the mutation (Undo)
    virtual void Revert() override;

public:
    UOdysseyAnimationPlayer* mPlayer;
    int mFrame;
};
