// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
    FOdysseyImageLayer( FName iName, FVector2D iSize );
    FOdysseyImageLayer( FName iName, FOdysseyBlock* iBlock );

public:
    // Public API
    virtual eType           GetType()           const override;
    FOdysseyBlock*          GetBlock()          const;
    ::ULIS::eBlendingMode   GetBlendingMode()   const;

    void            SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode );
    void            SetBlendingMode( FText iBlendingMode );

    FText           GetBlendingModeAsText() const;

    float           GetOpacity() const;
    void            SetOpacity( float iOpacity );

    void            CopyPropertiesFrom( const FOdysseyImageLayer &iCopy );

private:
    // Private Data Members
    FOdysseyBlock*          mBlock;
    ::ULIS::eBlendingMode   mBlendingMode;
    float                   mOpacity;
};

