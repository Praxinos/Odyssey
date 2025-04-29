// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyMedia.h"

class FOdysseyVectorGroupPaint;

class ODYSSEYMEDIA_API FOdysseyMediaVector
    : public IOdysseyMedia
{
public:
    DECLARE_ODYSSEY_MEDIA()

public:
    //Destructor
    virtual ~FOdysseyMediaVector();
    FOdysseyMediaVector(FOdysseyVectorGroupPaint* iScene);

public:
    //IOdysseyMedia API
    FOdysseyVectorGroupPaint* GetScene() const;

private:
    FOdysseyVectorGroupPaint* mScene;
};
