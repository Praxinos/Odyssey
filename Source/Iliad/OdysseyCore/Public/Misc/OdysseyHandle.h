// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

class ODYSSEYCORE_API IOdysseyHandle
{
};

class ODYSSEYCORE_API FOdysseyHandleContainer : public IOdysseyHandle
{
public:
    virtual ~FOdysseyHandleContainer();
    FOdysseyHandleContainer(const TArray<TSharedPtr<IOdysseyHandle>>& iHandles);

private:
    TArray<TSharedPtr<IOdysseyHandle>> mHandles;
};
