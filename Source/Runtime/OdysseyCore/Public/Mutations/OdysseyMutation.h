// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYCORE_API IOdysseyMutation
{
public:
    //A unique identifier for the mutation type
    virtual const FGuid& GetID() = 0;

    //Applies the mutation
    virtual void Apply() = 0;

    //Reverts the mutation (Undo)
    virtual void Revert() = 0;
};