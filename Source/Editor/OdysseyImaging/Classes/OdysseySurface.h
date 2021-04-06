// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

class FOdysseyBlock;

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API IOdysseySurface
{
public:
    virtual ~IOdysseySurface() {};

public:
    // Public API
    virtual UTexture*             Texture() = 0; //TODO: Change to UTexture*
    virtual const UTexture*       Texture() const = 0; //TODO: Change to UTexture*

public:
    // Public Tampon Methods
    virtual int  Width() = 0;
    virtual int  Height() = 0;
};
