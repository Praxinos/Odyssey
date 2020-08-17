// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "OdysseySurfaceReadOnly.h"

/////////////////////////////////////////////////////
// FOdysseySurfaceReadOnly
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySurfaceReadOnly::~FOdysseySurfaceReadOnly()
{
    if (mTexture) {
        mTexture->RemoveFromRoot();
    }
}

FOdysseySurfaceReadOnly::FOdysseySurfaceReadOnly(UTexture2D* iTexture)
	: mTexture(iTexture)
{
	if (mTexture)
	{
		mTexture->AddToRoot();
	}
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

UTexture2D*
FOdysseySurfaceReadOnly::Texture()
{
    return mTexture;
}

const UTexture2D*
FOdysseySurfaceReadOnly::Texture() const
{
    return mTexture;
}

void
FOdysseySurfaceReadOnly::Texture(UTexture2D* iTexture)
{
    if (mTexture)
    {
        mTexture->RemoveFromRoot();
    }

    mTexture = iTexture;

	if (mTexture)
	{
		mTexture->AddToRoot();
	}
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- Public Tampon Methods
int
FOdysseySurfaceReadOnly::Width()
{
	if (!mTexture)
		return 0;
    return mTexture->GetSizeX();
}

int
FOdysseySurfaceReadOnly::Height()
{
	if (!mTexture)
		return 0;
    return mTexture->GetSizeY();
}
