// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IOdysseyLayer.h"
#include <ULIS_BLENDINGMODES>

class FOdysseyBlock;

/**
 * Implements a layer which is a folder
 */
class ODYSSEYLAYER_API FOdysseyFolderLayer : public IOdysseyLayer
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFolderLayer();
    FOdysseyFolderLayer( const FName& iName );

public:
    virtual eType GetType() const override;

public:
    // Public API
    ::ULIS::eBlendingMode     GetBlendingMode();
    FText                     GetBlendingModeAsText() const;
    void                      SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode );
    void                      SetBlendingMode( FText iBlendingMode );
    
    float GetOpacity() const;
    void  SetOpacity( float iOpacity );
    
    bool IsOpen() const;
    void SetIsOpen( bool iIsOpen );

private:
    ::ULIS::eBlendingMode               mBlendingMode;
    float                               mOpacity;
    
    //To do: Not good, should not be here in the model. To change after the dissociation between the model and the view
    bool                                mIsOpen;
};
