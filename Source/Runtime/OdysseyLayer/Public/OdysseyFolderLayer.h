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
class ODYSSEYLAYER_API FOdysseyFolderLayer : public IOdysseyLayer
{
public:
    // Construction / Destruction
    FOdysseyFolderLayer( FName InName );
    virtual ~FOdysseyFolderLayer();

public:
    // Public API
    virtual eType                       GetType() const override;
    ::ULIS::eBlendingMode               GetBlendingMode();
    void                                SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode );
    TSharedPtr<FOdysseyBlock>           GenerateBlockFromContent() const;
    void                                AppendLayer( TSharedPtr<IOdysseyLayer> iLayer );
    void                                AddLayerAtIndex( TSharedPtr<IOdysseyLayer> iLayer, int iIndex );

private:
    TArray<TSharedPtr<IOdysseyLayer>>   mLayersInFolder;
    ::ULIS::eBlendingMode               mBlendingMode;
};

