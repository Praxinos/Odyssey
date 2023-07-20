// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyMedia.h"

class FOdysseyVectorScene;

class ODYSSEYMEDIA_API FOdysseyMediaVector
    : public IOdysseyMedia
{
public:
    DECLARE_ODYSSEY_MEDIA()

public:
    //Destructor
    virtual ~FOdysseyMediaVector();
    FOdysseyMediaVector(FOdysseyVectorScene* iScene);

public:
    //IOdysseyMedia API
    FOdysseyVectorScene* GetScene() const;

private:
    FOdysseyVectorScene* mScene;
};

