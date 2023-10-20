// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Mutations/OdysseyMutator.h"

class UObject;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageStaggerMutator
    : public FOdysseyMutator
{
public:
    FOdysseyAnimationCellImageStaggerMutator(UObject* iOwner, TSharedPtr<FOdysseyAnimationCellImageStagger> iCell);

public:
    void SetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour);
    void SetReach(int iReach);

private:
    TSharedPtr<FOdysseyAnimationCellImageStagger> mCell;
};