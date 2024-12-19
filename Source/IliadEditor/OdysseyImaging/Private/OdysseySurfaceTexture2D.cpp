// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#include "OdysseySurfaceTexture2D.h"

/////////////////////////////////////////////////////
// FOdysseySurfaceTexture2D
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySurfaceTexture2D::~FOdysseySurfaceTexture2D()
{
}

FOdysseySurfaceTexture2D::FOdysseySurfaceTexture2D(UTexture2D* iTexture)
    : mTexture(iTexture)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

UTexture2D*
FOdysseySurfaceTexture2D::Texture()
{
    return mTexture.Get();
}

const UTexture2D*
FOdysseySurfaceTexture2D::Texture() const
{
    return mTexture.Get();
}

void
FOdysseySurfaceTexture2D::Texture(UTexture2D* iTexture)
{
    mTexture = TStrongObjectPtr<UTexture2D>(iTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- Public Tampon Methods
int
FOdysseySurfaceTexture2D::Width()
{
    if (!mTexture)
        return 0;
    return mTexture->Source.GetSizeX();
}

int
FOdysseySurfaceTexture2D::Height()
{
    if (!mTexture)
        return 0;
    return mTexture->Source.GetSizeY();
}
