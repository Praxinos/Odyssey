// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
    IOdysseyLayer();
    virtual ~IOdysseyLayer() = 0;

public:
    FName           GetName() const;
    FText           GetNameAsText() const;
    void            SetName(FName iName);
    bool            IsLocked() const;
    void            SetIsLocked(bool iIsLocked);
    bool            IsVisible() const;
    void            SetIsVisible(bool iIsVisible);

    virtual eType   GetType() const = 0;

protected :
    FName           mName;
    bool            mIsLocked;
    bool            mIsVisible;
};

