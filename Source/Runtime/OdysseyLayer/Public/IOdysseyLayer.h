// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

class FOdysseyBlock;

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
    IOdysseyLayer();
    IOdysseyLayer( const FName& iName );

public:
    virtual eType GetType() const = 0;

    virtual FName GetName() const;
    virtual FText GetNameAsText() const;
    virtual void  SetName( FName iName );

    virtual bool  IsLocked() const;
    virtual void  SetIsLocked( bool iIsLocked );

    virtual bool  IsVisible() const;
    virtual void  SetIsVisible( bool iIsVisible );

protected:
    FName         mName;
    bool          mIsLocked;
    bool          mIsVisible;
};
