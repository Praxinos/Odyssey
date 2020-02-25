// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IOdysseyLayer.h"
#include <ULIS_BLENDINGMODES>


class FOdysseyBlock;

/**
 * Implements a layer which contains an image
 */
class ODYSSEYLAYER_API FOdysseyImageLayer : public IOdysseyLayer
{
public:
    // Construction / Destruction
    virtual ~FOdysseyImageLayer();
    FOdysseyImageLayer( const FName& iName, FVector2D iSize, ETextureSourceFormat iTextureSourceFormat );
    FOdysseyImageLayer( const FName& iName, FOdysseyBlock* iBlock );

public:
    // Public API
    FOdysseyBlock* GetBlock() const;

    ::ULIS::eBlendingMode GetBlendingMode() const;
    FText                 GetBlendingModeAsText() const;
    void                  SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode );
    void                  SetBlendingMode( FText iBlendingMode );

    float GetOpacity() const;
    void  SetOpacity( float iOpacity );
    
    bool  IsAlphaLocked() const;
    void  SetIsAlphaLocked( bool iIsAlphaLocked );
    
    // Overloads for save in archive
    friend FArchive& operator<<(FArchive &Ar, FOdysseyImageLayer* ioSaveImageLayer );

public:
    void CopyPropertiesFrom( const FOdysseyImageLayer &iCopy ); // TODO: replace it by a Clone()/copy-ctor/...

private:
    // Private Data Members
    FOdysseyBlock*          mBlock;
    ::ULIS::eBlendingMode   mBlendingMode;
    float                   mOpacity;
    bool                    mIsAlphaLocked;
};

//Serialization of item
#include "OdysseyImageLayer.inl"
