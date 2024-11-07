// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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