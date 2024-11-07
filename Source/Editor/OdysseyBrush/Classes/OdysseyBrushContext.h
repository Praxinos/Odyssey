// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

/////////////////////////////////////////////////////
// FOdysseyBrushState
class ODYSSEYBRUSH_API FOdysseyBrushContext
{
public:
    virtual ~FOdysseyBrushContext() = 0;
    FOdysseyBrushContext(const FString& iName);

public:
    const FString& Name() const;

private:
    FString mName;
};

