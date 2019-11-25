// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Engine/Texture.h"
#include "IOdysseyLayer.h"
#include <ULIS_BLENDINGMODES>

class FOdysseyBlock;
class UTexture2D;

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
    virtual eType GetType() const override;

public:
    // Public API
    FOdysseyBlock* GetBlock() const;

    ::ULIS::eBlendingMode GetBlendingMode() const;
    FText                 GetBlendingModeAsText() const;
    void                  SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode );
    void                  SetBlendingMode( FText iBlendingMode );

    float GetOpacity() const;
    void  SetOpacity( float iOpacity );

public:
    void CopyPropertiesFrom( const FOdysseyImageLayer &iCopy ); // TODO: replace it by a Clone()/copy-ctor/...

private:
    // Private Data Members
    FOdysseyBlock*          mBlock;
    ::ULIS::eBlendingMode   mBlendingMode;
    float                   mOpacity;
    
//public:
    //Temporary Undo necessity
    //UTexture2D*             mTexture;
};
