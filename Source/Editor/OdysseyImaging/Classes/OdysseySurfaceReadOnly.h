// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#pragma once
#include "OdysseySurface.h"

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API FOdysseySurfaceReadOnly :
    public IOdysseySurface
{
public:
    // Construction / Destruction

    // Destruction will delete texture if Transient ( not borrowed )
    virtual ~FOdysseySurfaceReadOnly();

    // Responsible for its underlyin FOdysseyBlock TArray allocation
    // With borrowed UTexture, not responsible for its Allocation / Deallocation
    FOdysseySurfaceReadOnly(UTexture2D* iTexture);

private:
    // Forbid Copy Constructor and Copy Assignement Operator
    FOdysseySurfaceReadOnly(const FOdysseyBlock&)            = delete;
    FOdysseySurfaceReadOnly& operator=(const FOdysseyBlock&) = delete;

public:
    // Public API
    virtual UTexture2D*             Texture();
    virtual const UTexture2D*       Texture() const;

    void Texture(UTexture2D* iTexture);

public:
    // Public Tampon Methods
    virtual int  Width();
    virtual int  Height();

private:
    // Private Data
    UPROPERTY()
    UTexture2D* mTexture;
};
