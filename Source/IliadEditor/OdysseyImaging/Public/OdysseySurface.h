// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API IOdysseySurface
{
public:
    virtual ~IOdysseySurface() {};

public:
    // Public API
    virtual UTexture*             Texture() = 0;
    virtual const UTexture*       Texture() const = 0;

public:
    // Public Tampon Methods
    virtual int  Width() = 0;
    virtual int  Height() = 0;
};
