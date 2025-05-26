// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "OdysseySurface.h"
#include "UObject/StrongObjectPtr.h"

namespace ULIS { class FBlock; }

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API FOdysseySurfaceTexture2D :
    public IOdysseySurface
{
public:
    // Construction / Destruction

    // Destruction will delete texture if Transient ( not borrowed )
    virtual ~FOdysseySurfaceTexture2D();

    // Responsible for its underlyin ::ULIS::FBlock TArray allocation
    // With borrowed UTexture, not responsible for its Allocation / Deallocation
    FOdysseySurfaceTexture2D(UTexture2D* iTexture);

private:
    // Forbid Copy Constructor and Copy Assignement Operator
    FOdysseySurfaceTexture2D(const ::ULIS::FBlock&)            = delete;
    FOdysseySurfaceTexture2D& operator=(const ::ULIS::FBlock&) = delete;

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
    TStrongObjectPtr<UTexture2D> mTexture;
};
