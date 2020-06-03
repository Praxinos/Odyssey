// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

class FOdysseyBlock;
class FOdysseyLayerStack;

/**
 * Odyssey Layer
 * An abstract class for a layer, which can be of various types (drawing, sound, folder...)
 */
class ODYSSEYLAYER_API IOdysseyLayer
{
public:
    enum class eType : char
    {
        kInvalid,
        kImage,
        kFolder,
    };

public:
    // Construction / Destruction
    virtual ~IOdysseyLayer() = 0;
    IOdysseyLayer( const eType type );
    IOdysseyLayer( const FName& iName, const eType type );

public:
    virtual eType GetType() const;

    virtual FName GetName() const;
    virtual FText GetNameAsText() const;
    virtual void  SetName( FName iName );

    virtual bool  IsLocked() const;
    virtual void  SetIsLocked( bool iIsLocked );

    virtual bool  IsVisible() const;
    virtual void  SetIsVisible( bool iIsVisible );

    // Overloads for save in archive
    friend ODYSSEYLAYER_API FArchive& operator<<(FArchive &Ar, IOdysseyLayer** ioSaveImageLayer );

protected:
    FName         mName;
    bool          mIsLocked;
    bool          mIsVisible;
    eType         mType;
};

ODYSSEYLAYER_API FArchive& operator<<(FArchive &Ar, IOdysseyLayer** ioSaveImageLayer );
