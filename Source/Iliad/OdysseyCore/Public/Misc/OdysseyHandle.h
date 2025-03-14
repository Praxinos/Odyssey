// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
