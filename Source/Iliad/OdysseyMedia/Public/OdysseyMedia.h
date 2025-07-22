// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#define DECLARE_ODYSSEY_MEDIA() \
static const FGuid& StaticId(); \
virtual const FGuid& Id() override;

#define IMPLEMENT_ODYSSEY_MEDIA(ClassName) \
const FGuid& ClassName::StaticId() { static FGuid id = FGuid::NewGuid(); return id; } \
const FGuid& ClassName::Id() { return StaticId(); }

class ODYSSEYMEDIA_API IOdysseyMedia
{
public:
    //Destructor
    virtual ~IOdysseyMedia();

public:
    //IOdysseyMedia API

    //Must return a unique Id identifying the class
    virtual const FGuid& Id() = 0;

    //bool IsLocked() const;
    //void IsLocked(bool iIsLocked);

private:
    //bool mIsLocked;
};
