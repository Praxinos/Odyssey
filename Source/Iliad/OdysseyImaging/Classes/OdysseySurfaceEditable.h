// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "OdysseySurface.h"
#include <ULIS>

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API IOdysseySurfaceEditable : public IOdysseySurface
{
public:
    virtual ~IOdysseySurfaceEditable() {};
public:
    // Public API
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>    Block() const = 0;
    virtual void Invalidate() = 0;
    //virtual void Invalidate(int iX1,int iY1,int iX2,int iY2) = 0;
    //virtual void Invalidate(const ::ULIS::FRectI& iRect) = 0;
    virtual void Invalidate( const TArray<::ULIS::FRectI>& iRects ) = 0;
};
