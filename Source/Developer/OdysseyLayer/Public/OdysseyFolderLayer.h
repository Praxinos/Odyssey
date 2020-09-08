// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IOdysseyLayer.h"
#include <ULIS3>


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
    // Public API
    ::ul3::eBlendingMode    GetBlendingMode();
    FText                   GetBlendingModeAsText() const;
    void                    SetBlendingMode( ::ul3::eBlendingMode iBlendingMode );
    void                    SetBlendingMode( FText iBlendingMode );

    float GetOpacity() const;
    void  SetOpacity( float iOpacity );

    bool IsOpen() const;
    void SetIsOpen( bool iIsOpen );

    // Overloads for save in archive
    friend ODYSSEYLAYER_API FArchive& operator<<(FArchive &Ar, FOdysseyFolderLayer* ioSaveFolderLayer );

private:
    ::ul3::eBlendingMode    mBlendingMode;
    float                   mOpacity;
    bool                    mIsOpen; // Todo: This should not be here in the model, but it should be part of the view.
};

ODYSSEYLAYER_API FArchive& operator<<( FArchive &Ar, FOdysseyFolderLayer* ioSaveFolderLayer );