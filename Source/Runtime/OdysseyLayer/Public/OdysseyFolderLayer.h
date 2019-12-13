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
    void                      SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode );

    TSharedPtr<FOdysseyBlock> GenerateBlockFromContent() const;

    void                      AppendLayer( TSharedPtr<IOdysseyLayer> iLayer );
    void                      AddLayerAtIndex( TSharedPtr<IOdysseyLayer> iLayer, int iIndex );

private:
    TArray<TSharedPtr<IOdysseyLayer>>   mLayersInFolder;
    ::ULIS::eBlendingMode               mBlendingMode;
};
