// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

class FOdysseySurface;
class FOdysseyBlock;

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API IOdysseySurface
{
public:
    // Public API
    virtual UTexture2D*             Texture() = 0;
    virtual const UTexture2D*       Texture() const = 0;

public:
    // Public Tampon Methods
    virtual int  Width() = 0;
    virtual int  Height() = 0;
};
