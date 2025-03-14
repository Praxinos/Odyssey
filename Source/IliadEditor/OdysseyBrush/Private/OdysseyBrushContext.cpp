// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyBrushContext.h"

//---

FOdysseyBrushContext::~FOdysseyBrushContext()
{
}

FOdysseyBrushContext::FOdysseyBrushContext(const FString& iName)
    : mName(iName)
{
}

const FString&
FOdysseyBrushContext::Name() const
{
    return mName;
}
